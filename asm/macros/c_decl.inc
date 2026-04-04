// NOTE:
// For this to work, the file that is including this file HAS to be passed through the C preprocessor before assembling!

// 32bit Windows, macOS, and the Saturn SH-2 toolchain prefix labels using an underscore.
// For interoperability we need to replicate that behavior here.
#if ( (CPU_ARCH_X86 && defined(_WIN32) && !defined(_WIN64)) || defined(__APPLE__) || CPU_ARCH_SH2 )
    #define C_DECL(label) _##label
#else
    #define C_DECL(label) label
#endif
