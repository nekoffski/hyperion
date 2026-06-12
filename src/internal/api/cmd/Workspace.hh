#pragma once

#include "internal/api/Message.hh"
#include "internal/models/Workspace.hh"

namespace hyperion::api {

class WorkspaceCreateRequest
    : public ApiMessageImpl<
          WorkspaceCreateRequest, ApiMessageKind::workspaceCreateRequest> {
   public:
    WorkspaceCreateRequest();
    explicit WorkspaceCreateRequest(const WorkspaceConfig& config);

    const WorkspaceConfig& config() const;
    WorkspaceConfig& config();

    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

   private:
    WorkspaceConfig m_config;
};

class WorkspaceDeleteRequest
    : public ApiMessageImpl<
          WorkspaceDeleteRequest, ApiMessageKind::workspaceDeleteRequest> {
   public:
    WorkspaceDeleteRequest();
    explicit WorkspaceDeleteRequest(const Str& name);

    const Str& workspaceName() const;
    Str& workspaceName();

    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

   private:
    Str m_workspaceName;
};

class WorkspaceListRequest
    : public ApiMessageImpl<
          WorkspaceListRequest, ApiMessageKind::workspaceListRequest> {
   public:
    WorkspaceListRequest();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;
};

class WorkspaceListResponse
    : public ApiMessageImpl<
          WorkspaceListResponse, ApiMessageKind::workspaceListResponse> {
   public:
    WorkspaceListResponse();

    explicit WorkspaceListResponse(
        const std::vector<WorkspaceConfig>& workspaces
    );

    const std::vector<WorkspaceConfig>& workspaces() const;
    std::vector<WorkspaceConfig>& workspaces();

    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

   private:
    std::vector<WorkspaceConfig> m_workspaces;
};

}  // namespace hyperion::api
