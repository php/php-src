all: all-recursive

include $(builddir)/libs.mk

include $(top_srcdir)/build/rules.mk
