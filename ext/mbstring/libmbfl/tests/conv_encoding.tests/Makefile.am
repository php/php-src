EXTRA_DIST=\
	cp51932_cp50220raw.exp\
	ujis_sjis.exp\
	utf8_sjis.exp\
	yensign.exp
noinst_PROGRAMS=gen_exp
gen_exp_SOURCES=gen_exp.c
CLEANFILES=\
	uni_to_cp932.exp\
	uni_to_cp50220.exp\
	uni_to_cp50222.exp\
	cp932_to_uni.exp

CP932.TXT:
	$(FETCH_VIA_FTP) ftp://unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP932.TXT

cp932_to_uni.exp: gen_exp CP932.TXT
	./gen_exp from_cp932 > "$@"

uni_to_cp932.exp: gen_exp CP932.TXT
	./gen_exp to_cp932 > "$@"

uni_to_cp50220.exp: gen_exp CP932.TXT
	./gen_exp to_cp50220 > "$@"

uni_to_cp50222.exp: gen_exp CP932.TXT
	./gen_exp to_cp50222 > "$@"

check-local: uni_to_cp932.exp uni_to_cp50220.exp uni_to_cp50222.exp

.PHONY: check-local
