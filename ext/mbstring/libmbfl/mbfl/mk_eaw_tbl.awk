#!/usr/bin/awk -f
#
# $Id$
#
# Description: a script to generate east asian width table.
#

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
		v = ( "0x" $1 ) + 0
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
		vs = ( "0x" $1 ) + 0
		ve = ( "0x" $3 ) + 0
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
