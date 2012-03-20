RAGEL = ragel

.SUFFIXES: .rl

.rl.c:
	$(RAGEL) -G2 $<
	$(BASERUBY) -pli -e '$$_.sub!(/[ \t]+$$/, "")' $@

parser.c:
