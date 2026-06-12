#include <gtest/gtest.h>

#include "internal/api/cmd/Ack.hh"
#include "internal/api/cmd/Error.hh"
#include "internal/api/cmd/Health.hh"
#include "internal/api/cmd/Job.hh"
#include "internal/api/cmd/Workspace.hh"

using namespace hyperion;
using namespace hyperion::api;

// Serialize a message into an owned buffer, skip the kind prefix, and expose
// a PayloadReader ready for deserializeImpl.  All three variables must live in
// the same scope so the reader's span stays valid.
#define BEGIN_ROUNDTRIP(src)                        \
    PayloadWriter writer_;                          \
    (src).serialize(writer_);                       \
    auto view_ = writer_.getBuffer();               \
    PayloadBuffer buf_{view_.begin(), view_.end()}; \
    PayloadReader reader_{buf_};                    \
    reader_.read<MessageKindUnderlying>() /* consume kind prefix */

// ─── Ack ─────────────────────────────────────────────────────────────────────

TEST(ApiCmdAck, KindIsCorrect) { EXPECT_EQ(Ack{}.kind(), ApiMessageKind::ack); }

TEST(ApiCmdAck, RoundtripNoPayload) {
    Ack src;
    BEGIN_ROUNDTRIP(src);
    Ack dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.kind(), ApiMessageKind::ack);
}

// ─── ErrorResponse ───────────────────────────────────────────────────────────

TEST(ApiCmdErrorResponse, KindIsCorrect) {
    EXPECT_EQ(ErrorResponse{}.kind(), ApiMessageKind::errorResponse);
}

TEST(ApiCmdErrorResponse, RoundtripDetailsAndCode) {
    ErrorResponse src{"something went wrong", ErrorCode::ioError};
    BEGIN_ROUNDTRIP(src);
    ErrorResponse dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.details(), "something went wrong");
    EXPECT_EQ(dst.code(), ErrorCode::ioError);
}

TEST(ApiCmdErrorResponse, RoundtripFromException) {
    Exception ex{ErrorCode::invalidArgument, "bad arg"};
    ErrorResponse src{ex};
    BEGIN_ROUNDTRIP(src);
    ErrorResponse dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.details(), std::string{ex.what()});
    EXPECT_EQ(dst.code(), ErrorCode::invalidArgument);
}

TEST(ApiCmdErrorResponse, RoundtripEmptyDetails) {
    ErrorResponse src{"", ErrorCode::none};
    BEGIN_ROUNDTRIP(src);
    ErrorResponse dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.details(), "");
    EXPECT_EQ(dst.code(), ErrorCode::none);
}

// ─── HealthRequest ───────────────────────────────────────────────────────────

TEST(ApiCmdHealthRequest, KindIsCorrect) {
    EXPECT_EQ(HealthRequest{}.kind(), ApiMessageKind::healthRequest);
}

TEST(ApiCmdHealthRequest, RoundtripUuid) {
    HealthRequest src;
    src.uuid() = "test-uuid-1234";
    BEGIN_ROUNDTRIP(src);
    HealthRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.uuid(), "test-uuid-1234");
}

// ─── HealthResponse ──────────────────────────────────────────────────────────

TEST(ApiCmdHealthResponse, KindIsCorrect) {
    EXPECT_EQ(HealthResponse{}.kind(), ApiMessageKind::healthResponse);
}

TEST(ApiCmdHealthResponse, RoundtripUuid) {
    HealthResponse src{"echo-uuid-5678"};
    BEGIN_ROUNDTRIP(src);
    HealthResponse dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.uuid(), "echo-uuid-5678");
}

// ─── WorkspaceCreateRequest ──────────────────────────────────────────────────

TEST(ApiCmdWorkspaceCreateRequest, KindIsCorrect) {
    EXPECT_EQ(
        WorkspaceCreateRequest{}.kind(), ApiMessageKind::workspaceCreateRequest
    );
}

TEST(ApiCmdWorkspaceCreateRequest, RoundtripConfigName) {
    WorkspaceCreateRequest src{WorkspaceConfig{"my-workspace"}};
    BEGIN_ROUNDTRIP(src);
    WorkspaceCreateRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.config().name, "my-workspace");
}

// ─── WorkspaceDeleteRequest ──────────────────────────────────────────────────

TEST(ApiCmdWorkspaceDeleteRequest, KindIsCorrect) {
    EXPECT_EQ(
        WorkspaceDeleteRequest{}.kind(), ApiMessageKind::workspaceDeleteRequest
    );
}

TEST(ApiCmdWorkspaceDeleteRequest, RoundtripName) {
    WorkspaceDeleteRequest src{"workspace-to-delete"};
    BEGIN_ROUNDTRIP(src);
    WorkspaceDeleteRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.workspaceName(), "workspace-to-delete");
}

// ─── WorkspaceListRequest ────────────────────────────────────────────────────

TEST(ApiCmdWorkspaceListRequest, KindIsCorrect) {
    EXPECT_EQ(
        WorkspaceListRequest{}.kind(), ApiMessageKind::workspaceListRequest
    );
}

TEST(ApiCmdWorkspaceListRequest, RoundtripNoPayload) {
    WorkspaceListRequest src;
    BEGIN_ROUNDTRIP(src);
    WorkspaceListRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.kind(), ApiMessageKind::workspaceListRequest);
}

// ─── WorkspaceListResponse ───────────────────────────────────────────────────

TEST(ApiCmdWorkspaceListResponse, KindIsCorrect) {
    EXPECT_EQ(
        WorkspaceListResponse{}.kind(), ApiMessageKind::workspaceListResponse
    );
}

TEST(ApiCmdWorkspaceListResponse, RoundtripEmptyList) {
    WorkspaceListResponse src{{}};
    BEGIN_ROUNDTRIP(src);
    WorkspaceListResponse dst;
    dst.deserialize(reader_);
    EXPECT_TRUE(dst.workspaces().empty());
}

TEST(ApiCmdWorkspaceListResponse, RoundtripMultipleWorkspaces) {
    WorkspaceListResponse src{
        std::vector<WorkspaceConfig>{{"ws-a"}, {"ws-b"}, {"ws-c"}}
    };
    BEGIN_ROUNDTRIP(src);
    WorkspaceListResponse dst;
    dst.deserialize(reader_);
    ASSERT_EQ(dst.workspaces().size(), 3u);
    EXPECT_EQ(dst.workspaces()[0].name, "ws-a");
    EXPECT_EQ(dst.workspaces()[1].name, "ws-b");
    EXPECT_EQ(dst.workspaces()[2].name, "ws-c");
}

// ─── JobCreateRequest ────────────────────────────────────────────────────────

TEST(ApiCmdJobCreateRequest, KindIsCorrect) {
    EXPECT_EQ(JobCreateRequest{}.kind(), ApiMessageKind::jobCreateRequest);
}

TEST(ApiCmdJobCreateRequest, RoundtripTypeAndArgs) {
    JobArgs args;
    args.insert("key1", "val1");
    args.insert("key2", "val2");
    JobCreateRequest src{JobConfig{JobType::delayedNoop, args}};
    BEGIN_ROUNDTRIP(src);
    JobCreateRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.config().type, JobType::delayedNoop);
    ASSERT_EQ(dst.config().args.size(), 2u);
    EXPECT_EQ(dst.config().args.at("key1"), "val1");
    EXPECT_EQ(dst.config().args.at("key2"), "val2");
}

TEST(ApiCmdJobCreateRequest, RoundtripEmptyArgs) {
    JobCreateRequest src{JobConfig{JobType::noop, {}}};
    BEGIN_ROUNDTRIP(src);
    JobCreateRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.config().type, JobType::noop);
    EXPECT_TRUE(dst.config().args.empty());
}

// ─── JobStopRequest ──────────────────────────────────────────────────────────

TEST(ApiCmdJobStopRequest, KindIsCorrect) {
    EXPECT_EQ(JobStopRequest{}.kind(), ApiMessageKind::jobStopRequest);
}

TEST(ApiCmdJobStopRequest, RoundtripUuid) {
    JobStopRequest src{"job-uuid-stop"};
    BEGIN_ROUNDTRIP(src);
    JobStopRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.uuid(), "job-uuid-stop");
}

// ─── JobStatusRequest ────────────────────────────────────────────────────────

TEST(ApiCmdJobStatusRequest, KindIsCorrect) {
    EXPECT_EQ(JobStatusRequest{}.kind(), ApiMessageKind::jobStatusRequest);
}

TEST(ApiCmdJobStatusRequest, RoundtripUuid) {
    JobStatusRequest src{"job-uuid-status"};
    BEGIN_ROUNDTRIP(src);
    JobStatusRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.uuid(), "job-uuid-status");
}

// ─── JobStatusResponse ───────────────────────────────────────────────────────

TEST(ApiCmdJobStatusResponse, KindIsCorrect) {
    EXPECT_EQ(JobStatusResponse{}.kind(), ApiMessageKind::jobStatusResponse);
}

TEST(ApiCmdJobStatusResponse, RoundtripDescription) {
    JobArgs args;
    args.insert("k", "v");
    JobStatusResponse src{JobDescription{
        JobType::executeKernel, JobStatus::running, "uuid-xyz", args
    }};
    BEGIN_ROUNDTRIP(src);
    JobStatusResponse dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.description().type, JobType::executeKernel);
    EXPECT_EQ(dst.description().status, JobStatus::running);
    EXPECT_EQ(dst.description().uuid, "uuid-xyz");
    ASSERT_EQ(dst.description().args.size(), 1u);
    EXPECT_EQ(dst.description().args.at("k"), "v");
}

TEST(ApiCmdJobStatusResponse, RoundtripEmptyArgs) {
    JobStatusResponse src{
        JobDescription{JobType::noop, JobStatus::completed, "uuid-done", {}}
    };
    BEGIN_ROUNDTRIP(src);
    JobStatusResponse dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.description().type, JobType::noop);
    EXPECT_EQ(dst.description().status, JobStatus::completed);
    EXPECT_EQ(dst.description().uuid, "uuid-done");
    EXPECT_TRUE(dst.description().args.empty());
}

// ─── JobListRequest ──────────────────────────────────────────────────────────

TEST(ApiCmdJobListRequest, KindIsCorrect) {
    EXPECT_EQ(JobListRequest{}.kind(), ApiMessageKind::jobListRequest);
}

TEST(ApiCmdJobListRequest, RoundtripNoPayload) {
    JobListRequest src;
    BEGIN_ROUNDTRIP(src);
    JobListRequest dst;
    dst.deserialize(reader_);
    EXPECT_EQ(dst.kind(), ApiMessageKind::jobListRequest);
}

// ─── JobListResponse ─────────────────────────────────────────────────────────

TEST(ApiCmdJobListResponse, KindIsCorrect) {
    EXPECT_EQ(JobListResponse{}.kind(), ApiMessageKind::jobListResponse);
}

TEST(ApiCmdJobListResponse, RoundtripEmptyList) {
    JobListResponse src{{}};
    BEGIN_ROUNDTRIP(src);
    JobListResponse dst;
    dst.deserialize(reader_);
    EXPECT_TRUE(dst.jobs().empty());
}

TEST(ApiCmdJobListResponse, RoundtripMultipleJobs) {
    std::vector<JobShortDescription> jobs{
        {JobType::noop, JobStatus::completed, "uuid-1"},
        {JobType::delayedNoop, JobStatus::failed, "uuid-2"},
        {JobType::executeKernel, JobStatus::running, "uuid-3"},
    };
    JobListResponse src{jobs};
    BEGIN_ROUNDTRIP(src);
    JobListResponse dst;
    dst.deserialize(reader_);
    ASSERT_EQ(dst.jobs().size(), 3u);
    EXPECT_EQ(dst.jobs()[0].uuid, "uuid-1");
    EXPECT_EQ(dst.jobs()[0].status, JobStatus::completed);
    EXPECT_EQ(dst.jobs()[0].type, JobType::noop);
    EXPECT_EQ(dst.jobs()[1].uuid, "uuid-2");
    EXPECT_EQ(dst.jobs()[1].status, JobStatus::failed);
    EXPECT_EQ(dst.jobs()[1].type, JobType::delayedNoop);
    EXPECT_EQ(dst.jobs()[2].uuid, "uuid-3");
    EXPECT_EQ(dst.jobs()[2].status, JobStatus::running);
    EXPECT_EQ(dst.jobs()[2].type, JobType::executeKernel);
}
