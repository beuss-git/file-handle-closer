#pragma once

namespace utils {
class HandleGuard {
   public:
    explicit HandleGuard(HANDLE handle) : m_handle(handle) {}

    static HandleGuard create() { return HandleGuard(nullptr); }

    ~HandleGuard() {
        if (m_handle != nullptr) {
            CloseHandle(m_handle);
        }
    }

    HandleGuard(HandleGuard const&) = delete;

    HandleGuard& operator=(HandleGuard const&) = delete;

    explicit operator bool() const {
        return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE;
    }

    [[nodiscard]] HANDLE& get() { return m_handle; }

    [[nodiscard]] HANDLE get() const { return m_handle; }

   private:
    HANDLE m_handle;
};

}  // namespace utils
