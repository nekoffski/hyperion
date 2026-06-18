#pragma once

#include "internal/api/Message.hh"
#include "internal/models/Workspace.hh"

namespace hyperion::api {

class KernelCreateRequest
    : public ApiMessageImpl<
          KernelCreateRequest, ApiMessageKind::kernelCreateRequest> {
   public:
    explicit KernelCreateRequest();

    const Str& workspaceName() const;
    const Str& kernelName() const;
    const Str& kernelSource() const;

    Str& workspaceName();
    Str& kernelName();
    Str& kernelSource();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    Str m_workspaceName;
    Str m_kernelName;
    Str m_kernelSource;
};

class KernelRemoveRequest
    : public ApiMessageImpl<
          KernelRemoveRequest, ApiMessageKind::kernelRemoveRequest> {
   public:
    explicit KernelRemoveRequest();

    const Str& workspaceName() const;
    const Str& kernelName() const;

    Str& workspaceName();
    Str& kernelName();

   private:
    void serializeImpl(PayloadWriter& writer) const final override;
    void deserializeImpl(PayloadReader& reader) final override;

    Str m_workspaceName;
    Str m_kernelName;
};

class KernelListRequest {};
class KernelListResponse {};

}  // namespace hyperion::api
