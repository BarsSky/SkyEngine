#ifndef LIBSKYENGINE_IMPORT_EXPORT_H
#define LIBSKYENGINE_IMPORT_EXPORT_H

#if  defined( _WIN32 ) && (_MSC_VER)
#   ifdef LIBSKYENGINE_EXPORTS
#       define LIBSKYENGINE_EXPORT __declspec(dllexport)
#   else
#       define LIBSKYENGINE_EXPORT __declspec(dllimport)
#   endif
#else
#       define LIBSKYENGINE_EXPORT
#endif
#endif // LIBSKYENGINE_IMPORT_EXPORT_H