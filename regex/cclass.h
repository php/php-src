/* character-class table */
static struct cclass {
	unsigned char *name;
	unsigned char *chars;
	unsigned char *multis;
} cclasses[] = {
	{"alnum",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",				""},
	{"alpha",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
					""},
	{"blank",	" \t",		""},
	{"cntrl",	"\007\b\t\n\v\f\r\1\2\3\4\5\6\16\17\20\21\22\23\24\
\25\26\27\30\31\32\33\34\35\36\37\177",	""},
	{"digit",	"0123456789",	""},
	{"graph",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\
0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~",
					""},
	{"lower",	"abcdefghijklmnopqrstuvwxyz",
					""},
	{"print",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\
0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ",
					""},
	{"punct",	"!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~",
					""},
	{"space",	"\t\n\v\f\r ",	""},
	{"upper",	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
					""},
	{"xdigit",	"0123456789ABCDEFabcdef",
					""},
	{NULL,		0,		""}
};
