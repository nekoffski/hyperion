#pragma once

#include "lib/api/Message.hh"
#include "lib/models/Job.hh"

namespace hyperion::api {

class JobCreateRequest
    : public ApiMessageImpl<
          JobCreateRequest, ApiMessageKind::jobCreateRequest> {
   public:
    JobCreateRequest();
    explicit JobCreateRequest(const JobConfig& config);

    const JobConfig& config() const;
    JobConfig& config();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    JobConfig m_config;
};

class JobStopRequest
    : public ApiMessageImpl<JobStopRequest, ApiMessageKind::jobStopRequest> {
   public:
    JobStopRequest();
    explicit JobStopRequest(const Str& uuid);

    const Str& uuid() const;
    Str& uuid();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    Str m_uuid;
};

class JobStatusRequest
    : public ApiMessageImpl<
          JobStatusRequest, ApiMessageKind::jobStatusRequest> {
   public:
    JobStatusRequest();
    explicit JobStatusRequest(const Str& uuid);

    const Str& uuid() const;
    Str& uuid();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    Str m_uuid;
};

class JobStatusResponse
    : public ApiMessageImpl<
          JobStatusResponse, ApiMessageKind::jobStatusResponse> {
   public:
    JobStatusResponse();
    explicit JobStatusResponse(const JobDescription& description);

    const JobDescription& description() const;
    JobDescription& description();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    JobDescription m_description;
};

class JobListRequest
    : public ApiMessageImpl<JobListRequest, ApiMessageKind::jobListRequest> {
   public:
    JobListRequest();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;
};

class JobListResponse
    : public ApiMessageImpl<JobListResponse, ApiMessageKind::jobListResponse> {
   public:
    JobListResponse();
    explicit JobListResponse(const std::vector<JobShortDescription>& jobs);

    const std::vector<JobShortDescription>& jobs() const;
    std::vector<JobShortDescription>& jobs();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    std::vector<JobShortDescription> m_jobs;
};

}  // namespace hyperion::api
