BEGIN {
	mode=0
	sources=""
}
	
mode == 0 && /^LTLIBRARY_SOURCES.*\\$/ {
	if (match($0, "[^=]*$")) {
	sources=substr($0, RSTART, RLENGTH-1)
	}
	mode=1
	next
}

mode == 0 && /^LTLIBRARY_SOURCES.*/ {
	if (match($0, "[^=]*$")) {
	sources=substr($0, RSTART, RLENGTH)
	}
}

mode == 1 && /.*\\$/ {
	sources=sources substr($0, 0, length - 1)
	next
}

mode == 1 {
	sources=sources $0
	mode=0
}

END {
	print sources
}
