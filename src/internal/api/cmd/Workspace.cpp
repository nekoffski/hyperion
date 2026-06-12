#include "Workspace.hh"

namespace hyperion::api {

WorkspaceCreateRequest::WorkspaceCreateRequest()
    : ApiMessageImpl<
          WorkspaceCreateRequest, ApiMessageKind::workspaceCreateRequest>(
          "WorkspaceCreateRequest"
      ) {}

WorkspaceCreateRequest::WorkspaceCreateRequest(const WorkspaceConfig& config)
    : ApiMessageImpl<
          WorkspaceCreateRequest, ApiMessageKind::workspaceCreateRequest>(
          "WorkspaceCreateRequest"
      ),
      m_config(config) {}

const WorkspaceConfig& WorkspaceCreateRequest::config() const {
    return m_config;
}

WorkspaceConfig& WorkspaceCreateRequest::config() { return m_config; }

void WorkspaceCreateRequest::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_config.name);
}

void WorkspaceCreateRequest::deserializeImpl(PayloadReader& reader) {
    reader.read(m_config.name);
}

WorkspaceDeleteRequest::WorkspaceDeleteRequest()
    : ApiMessageImpl<
          WorkspaceDeleteRequest, ApiMessageKind::workspaceDeleteRequest>(
          "WorkspaceDeleteRequest"
      ) {}

WorkspaceDeleteRequest::WorkspaceDeleteRequest(const Str& name)
    : ApiMessageImpl<
          WorkspaceDeleteRequest, ApiMessageKind::workspaceDeleteRequest>(
          "WorkspaceDeleteRequest"
      ),
      m_workspaceName(name) {}

const Str& WorkspaceDeleteRequest::workspaceName() const {
    return m_workspaceName;
}

Str& WorkspaceDeleteRequest::workspaceName() { return m_workspaceName; }

void WorkspaceDeleteRequest::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_workspaceName);
}

void WorkspaceDeleteRequest::deserializeImpl(PayloadReader& reader) {
    reader.read(m_workspaceName);
}

WorkspaceListRequest::WorkspaceListRequest()
    : ApiMessageImpl<
          WorkspaceListRequest, ApiMessageKind::workspaceListRequest>(
          "WorkspaceListRequest"
      ) {}

void WorkspaceListRequest::serializeImpl(PayloadWriter&) const {}

void WorkspaceListRequest::deserializeImpl(PayloadReader&) {}

WorkspaceListResponse::WorkspaceListResponse()
    : ApiMessageImpl<
          WorkspaceListResponse, ApiMessageKind::workspaceListResponse>(
          "WorkspaceListResponse"
      ) {}

WorkspaceListResponse::WorkspaceListResponse(
    const std::vector<WorkspaceConfig>& workspaces
)
    : ApiMessageImpl<
          WorkspaceListResponse, ApiMessageKind::workspaceListResponse>(
          "WorkspaceListResponse"
      ),
      m_workspaces(workspaces) {}

const std::vector<WorkspaceConfig>& WorkspaceListResponse::workspaces() const {
    return m_workspaces;
}
std::vector<WorkspaceConfig>& WorkspaceListResponse::workspaces() {
    return m_workspaces;
}

void WorkspaceListResponse::serializeImpl(PayloadWriter& writer) const {
    writer.write(static_cast<u8>(m_workspaces.size()));
    for (const auto& workspace : m_workspaces) {
        writer.write(workspace.name);
    }
}

void WorkspaceListResponse::deserializeImpl(PayloadReader& reader) {
    const auto size = reader.read<u8>();
    m_workspaces.clear();
    m_workspaces.reserve(size);
    for (u8 i = 0; i < size; ++i) {
        WorkspaceConfig config;
        reader.read(config.name);
        m_workspaces.push_back(config);
    }
}

}  // namespace hyperion::api
