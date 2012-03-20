/********************************************************************
 Flush register windows on sparc.

 This function is in a separate file to prevent inlining. The "flushw"
 assembler instruction used on sparcv9 flushes all register windows
 except the current one, so if it is inlined, the current register
 window of the process executing the instruction will not be flushed
 correctly.

 See http://bugs.ruby-lang.org/issues/5244 for discussion.
*********************************************************************/
void rb_sparc_flush_register_windows(void)
{
    asm
#ifdef __GNUC__
    __volatile__
#endif

/* This condition should be in sync with one in configure.in */
#if defined(__sparcv9) || defined(__sparc_v9__) || defined(__arch64__)
# ifdef __GNUC__
    ("flushw" : : : "%o7")
# else
    ("flushw")
# endif /* __GNUC__ */
#else
    ("ta 0x03")
#endif
    ;
}
