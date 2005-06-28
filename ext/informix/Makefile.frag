
OVERALL_TARGET += $(srcdir)/ifx.c

$(srcdir)/ifx.c: $(srcdir)/ifx.ec
	(if test -d $(INFORMIXDIR); then \
	   THREADLIB=POSIX $(INFORMIXDIR)/bin/esql -e $(IFX_ESQL_FLAGS) $(srcdir)/ifx.ec; mv ifx.c $@; \
	   THREADLIB=POSIX $(INFORMIXDIR)/bin/esql -e $(IFX_ESQL_FLAGS) $(srcdir)/ifx.ec; \
           mv ifx.c $@ || true; \
	 else \
	   touch $@; \
	 fi)

