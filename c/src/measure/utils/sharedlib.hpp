#ifndef MEASURE_UTILS_SHAREDLIB_HPP
#define MEASURE_UTILS_SHAREDLIB_HPP

#include <filesystem>

#if defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>
#elif defined(_WIN64)
#include <windows.h>
#else
#error "Unsupported OS"
#endif

namespace am::utils {

	namespace details {
#if defined(__linux__) || defined(__APPLE__)
		using libhandle = void*;
		inline libhandle openlib(const std::filesystem::path& path) noexcept { return dlopen(path.c_str(), RTLD_LAZY); }
		inline void closelib(libhandle handle) noexcept { dlclose(handle); }
		template <typename T>
		inline T loadfromlib(libhandle handle, const std::string& name) noexcept {
			return reinterpret_cast<T>(dlsym(handle, name.c_str()));
		}
#elif defined(_WIN64)
		using libhandle = HINSTANCE;
		inline libhandle openlib(const std::filesystem::path& path) { return LoadLibrary((LPCSTR)path.c_str()); }
		inline void closelib(libhandle handle) noexcept { /** Not possible */ }
#else
#error "Unsupported OS"
#endif
	}; // namespace details

	struct SharedLib {
	private:
		details::libhandle handle;

		SharedLib(const SharedLib& other) = delete;
		SharedLib& operator=(const SharedLib& other) = delete;

		void destroy() {
			if (handle == nullptr)
				return;
			details::closelib(handle);
			handle = nullptr;
		}

	protected:
		template <typename T>
		constexpr T load(const std::string& name) noexcept {
			return details::loadfromlib<T>(handle, name);
		}

	public:
		SharedLib() noexcept : handle(nullptr) {}
		SharedLib(const std::filesystem::path& path) noexcept : handle(details::openlib(path)) {}
		SharedLib(SharedLib&& other) noexcept : handle(std::move(other.handle)) { other.handle = nullptr; }

		virtual ~SharedLib() { destroy(); }

		SharedLib& operator=(SharedLib&& other) noexcept {
			destroy();
			handle = std::move(other.handle);
			other.handle = nullptr;
			return *this;
		}

		bool good() const noexcept { return handle != nullptr; }

		operator bool() const noexcept { return good(); }
	};

} // namespace am::utils

#endif