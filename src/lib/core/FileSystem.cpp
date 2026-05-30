#include "FileSystem.hh"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace hyperion {

const std::string& Path::str() const { return m_path; }

bool Path::isFile() const { return fs::is_regular_file(m_path); }

bool Path::isDirectory() const { return fs::is_directory(m_path); }

Path Path::parent() const {
    auto parentPath = fs::path(m_path).parent_path();
    return Path{parentPath.string()};
}

Path Path::join(const Path& base, const Path& relative) {
    return Path{fs::path(base.str()) / fs::path(relative.str())};
}

bool Path::endsWith(const std::string& suffix) const {
    if (suffix.size() > m_path.size()) [[unlikely]]
        return false;
    return std::equal(suffix.rbegin(), suffix.rend(), m_path.rbegin());
}

void Path::append(const std::string& suffix) { m_path += suffix; }

File::File(const Path& path) : m_path(path) {}

const Path& File::path() const { return m_path; }

void File::append(const std::string& content) {
    std::ofstream file(m_path.str(), std::ios::app);
    if (!file.is_open()) {
        throw FileSystemError{
            ErrorCode::fileNotFound, "Failed to open file for appending"
        };
    }
    file << content;
}

void File::write(const std::string& content) {
    std::ofstream file(m_path.str(), std::ios::trunc);
    if (!file.is_open()) {
        throw FileSystemError{
            ErrorCode::fileNotFound, "Failed to open file for writing"
        };
    }
    file << content;
}

std::string File::read() const {
    std::ifstream file(m_path.str());
    if (!file.is_open()) {
        throw FileSystemError{
            ErrorCode::fileNotFound, "Failed to open file for reading"
        };
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> File::readLines() const {
    std::ifstream file(m_path.str());
    if (!file.is_open()) {
        throw FileSystemError{
            ErrorCode::fileNotFound, "Failed to open file for reading"
        };
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    return lines;
}

std::vector<u32> File::readBinary() const {
    std::ifstream file(m_path.str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw FileSystemError{
            ErrorCode::fileNotFound, "Failed to open file '{}' for reading",
            m_path.str()
        };
    }
    auto size = file.tellg();
    if (size % sizeof(u32) != 0) {
        throw FileSystemError{
            ErrorCode::invalidArgument,
            "File '{}' size is not a multiple of 4 bytes", m_path.str()
        };
    }
    file.seekg(0);
    std::vector<u32> buffer(static_cast<std::size_t>(size) / sizeof(u32));
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

void File::remove() {
    std::error_code ec;
    fs::remove(m_path.str(), ec);
    if (ec) {
        throw FileSystemError{
            ErrorCode::fileNotFound, "Failed to remove file '{}'", m_path.str()
        };
    }
}

}  // namespace hyperion
