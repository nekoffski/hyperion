#include "Job.hh"

namespace hyperion::api {

JobCreateRequest::JobCreateRequest()
    : ApiMessageImpl<JobCreateRequest, ApiMessageKind::jobCreateRequest>(
          "JobCreateRequest"
      ) {}

JobCreateRequest::JobCreateRequest(const JobConfig& config)
    : ApiMessageImpl<JobCreateRequest, ApiMessageKind::jobCreateRequest>(
          "JobCreateRequest"
      ),
      m_config(config) {}

const JobConfig& JobCreateRequest::config() const { return m_config; }

JobConfig& JobCreateRequest::config() { return m_config; }

void JobCreateRequest::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_config.args);
    writer.write(static_cast<u8>(m_config.type));
}

void JobCreateRequest::deserializeImpl(PayloadReader& reader) {
    reader.read(m_config.args);
    u8 type = reader.read<u8>();
    m_config.type = static_cast<JobType>(type);
}

JobStopRequest::JobStopRequest()
    : ApiMessageImpl<JobStopRequest, ApiMessageKind::jobStopRequest>(
          "JobStopRequest"
      ) {}

JobStopRequest::JobStopRequest(const Str& uuid)
    : ApiMessageImpl<JobStopRequest, ApiMessageKind::jobStopRequest>(
          "JobStopRequest"
      ),
      m_uuid(uuid) {}

const Str& JobStopRequest::uuid() const { return m_uuid; }
Str& JobStopRequest::uuid() { return m_uuid; }

void JobStopRequest::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_uuid);
}
void JobStopRequest::deserializeImpl(PayloadReader& reader) {
    reader.read(m_uuid);
}

JobStatusRequest::JobStatusRequest()
    : ApiMessageImpl<JobStatusRequest, ApiMessageKind::jobStatusRequest>(
          "JobStatusRequest"
      ) {}

JobStatusRequest::JobStatusRequest(const Str& uuid)
    : ApiMessageImpl<JobStatusRequest, ApiMessageKind::jobStatusRequest>(
          "JobStatusRequest"
      ),
      m_uuid(uuid) {}

const Str& JobStatusRequest::uuid() const { return m_uuid; }
Str& JobStatusRequest::uuid() { return m_uuid; }

void JobStatusRequest::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_uuid);
}

void JobStatusRequest::deserializeImpl(PayloadReader& reader) {
    reader.read(m_uuid);
}

JobStatusResponse::JobStatusResponse()
    : ApiMessageImpl<JobStatusResponse, ApiMessageKind::jobStatusResponse>(
          "JobStatusResponse"
      ) {}

JobStatusResponse::JobStatusResponse(const JobDescription& description)
    : ApiMessageImpl<JobStatusResponse, ApiMessageKind::jobStatusResponse>(
          "JobStatusResponse"
      ),
      m_description(description) {}

const JobDescription& JobStatusResponse::description() const {
    return m_description;
}
JobDescription& JobStatusResponse::description() { return m_description; }

void JobStatusResponse::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_description.args);
    writer.write(static_cast<u8>(m_description.status));
    writer.write(static_cast<u8>(m_description.type));
    writer.write(m_description.uuid);
}

void JobStatusResponse::deserializeImpl(PayloadReader& reader) {
    reader.read(m_description.args);
    u8 status = reader.read<u8>();
    m_description.status = static_cast<JobStatus>(status);
    u8 type = reader.read<u8>();
    m_description.type = static_cast<JobType>(type);
    reader.read(m_description.uuid);
}

JobListRequest::JobListRequest()
    : ApiMessageImpl<JobListRequest, ApiMessageKind::jobListRequest>(
          "JobListRequest"
      ) {}

void JobListRequest::serializeImpl(PayloadWriter&) const {}

void JobListRequest::deserializeImpl(PayloadReader&) {}

JobListResponse::JobListResponse()
    : ApiMessageImpl<JobListResponse, ApiMessageKind::jobListResponse>(
          "JobListResponse"
      ) {}

JobListResponse::JobListResponse(const std::vector<JobShortDescription>& jobs)
    : ApiMessageImpl<JobListResponse, ApiMessageKind::jobListResponse>(
          "JobListResponse"
      ),
      m_jobs(jobs) {}

const std::vector<JobShortDescription>& JobListResponse::jobs() const {
    return m_jobs;
}
std::vector<JobShortDescription>& JobListResponse::jobs() { return m_jobs; }

void JobListResponse::serializeImpl(PayloadWriter& writer) const {
    writer.write(static_cast<u32>(m_jobs.size()));
    for (const auto& job : m_jobs) {
        writer.write(job.uuid);
        writer.write(static_cast<u8>(job.status));
        writer.write(static_cast<u8>(job.type));
    }
}

void JobListResponse::deserializeImpl(PayloadReader& reader) {
    auto count = reader.read<u32>();
    m_jobs.reserve(count);

    for (u32 i = 0; i < count; ++i) {
        JobShortDescription job;
        reader.read(job.uuid);

        u8 status = reader.read<u8>();
        job.status = static_cast<JobStatus>(status);

        u8 type = reader.read<u8>();
        job.type = static_cast<JobType>(type);

        m_jobs.push_back(std::move(job));
    }
}

}  // namespace hyperion::api
