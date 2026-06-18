#include "Kernel.hh"

namespace hyperion::api {

KernelCreateRequest::KernelCreateRequest()
    : ApiMessageImpl<KernelCreateRequest, ApiMessageKind::kernelCreateRequest>(
          "KernelCreateRequest"
      ) {}

void KernelCreateRequest::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_workspaceName);
    writer.write(m_kernelName);
    writer.write(m_kernelSource);
}

void KernelCreateRequest::deserializeImpl(PayloadReader& reader) {
    reader.read(m_workspaceName);
    reader.read(m_kernelName);
    reader.read(m_kernelSource);
}

const Str& KernelCreateRequest::workspaceName() const {
    return m_workspaceName;
}

const Str& KernelCreateRequest::kernelName() const { return m_kernelName; }
const Str& KernelCreateRequest::kernelSource() const { return m_kernelSource; }

Str& KernelCreateRequest::workspaceName() { return m_workspaceName; }
Str& KernelCreateRequest::kernelName() { return m_kernelName; }
Str& KernelCreateRequest::kernelSource() { return m_kernelSource; }

KernelRemoveRequest::KernelRemoveRequest()
    : ApiMessageImpl<KernelRemoveRequest, ApiMessageKind::kernelRemoveRequest>(
          "KernelRemoveRequest"
      ) {}

void KernelRemoveRequest::serializeImpl(PayloadWriter& writer) const {
    writer.write(m_workspaceName);
    writer.write(m_kernelName);
}

void KernelRemoveRequest::deserializeImpl(PayloadReader& reader) {
    reader.read(m_workspaceName);
    reader.read(m_kernelName);
}

const Str& KernelRemoveRequest::workspaceName() const {
    return m_workspaceName;
}
const Str& KernelRemoveRequest::kernelName() const { return m_kernelName; }

Str& KernelRemoveRequest::workspaceName() { return m_workspaceName; }
Str& KernelRemoveRequest::kernelName() { return m_kernelName; }

}  // namespace hyperion::api
