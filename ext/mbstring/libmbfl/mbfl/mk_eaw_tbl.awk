#!/usr/bin/awk -f
#
# $Id$
#
# Description: a script to generate east asian width table.
#

function conv(str) {
	if (!match(str, "^0[xX]")) {
		return 0 + str
	}

	retval = 0

	for (i = 3; i <= length(str); i++) {
		n = index("0123456789abcdefABCDEF", substr(str, i, 1)) - 1

		if (n < 0) {
			return 0 + str;
		} else if (n >= 16) {
			n -= 6;
		}

		retval = retval * 16 + n
	}

	return retval
}

BEGIN {
	prev = -1
	comma = 0
	ORS = ""
	FS = "[;.|# ]"
	print "static const struct {\n\tint begin;\n\tint end;\n} " TABLE_NAME "[] = {\n\t"
}

/^#/ {
}

/^[0-9a-fA-F]+;/ {
	if ($2 == "W" || $2 == "F") {
		v = conv("0x" $1)
		if (prev < 0) {
			first = v
		} else if (v - prev > 1) {
			if (comma) {
				print ",\n\t"
			}
			printf("{ 0x%04x, 0x%04x }", first, prev)
			first = v
			comma = 1
		}
		prev = v
	} else {
		if (prev >= 0) {
			if (comma) {
				print ",\n\t"
			}
			printf("{ 0x%04x, 0x%04x }", first, prev)
			prev = -1
			comma = 1
		}
	}
}

/^[0-9a-fA-F]+\.\./ {
	if ($4 == "W" || $4 == "F") {
		vs = conv("0x" $1)
		ve = conv("0x" $3)
		if (prev < 0) {
			first = vs
		} else if (vs - prev > 1) {
			if (comma) {
				print ",\n\t"
			}
			printf("{ 0x%04x, 0x%04x }", first, prev)
			first = vs
			comma = 1
		}
		prev = ve
	} else {
		if (prev >= 0) {
			if (comma) {
				print ",\n\t"
			}
			printf("{ 0x%04x, 0x%04x }", first, prev)
			prev = -1
			comma = 1
		}
	}
}

END {
	if (prev >= 0) {
		if (comma) {
			print ",\n\t"
		}
		printf("{ 0x%04x, 0x%04x }", first, prev)
	}
	print "\n};\n"
} 
