#ifndef PM_IO_VULKAN_IMPORT_EXPORT_H
#define PM_IO_VULKAN_IMPORT_EXPORT_H

#if  defined( _WIN32 ) && (_MSC_VER)
#   ifdef PM_IO_VULKAN_EXPORTS
#       define PM_IO_VULKAN_EXPORT __declspec(dllexport)
#   else
#       define PM_IO_VULKAN_EXPORT __declspec(dllimport)
#   endif
#else
#       define PM_IO_VULKAN_EXPORT
#endif
#endif // PM_IO_VULKAN_IMPORT_EXPORT_H