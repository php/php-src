# $Id$

/^[ \t]*php3_*/ {
	phpcommand=substr($1,6)
	phpvalue=tolower($2)
	print "<IfModule mod_php3.c>"
	print $0
	print "</IfModule>"
	print "<IfModule mod_php4.c>"
	if (phpvalue=="on") {
		print "php_admin_flag " phpcommand " on"
	} else  if (phpvalue=="off") {
		print "php_admin_flag " phpcommand " off"
	} else {
		print "php_admin_value " phpcommand " " substr($0,index($0,$1)+length($1)+1)
	}
	print "</IfModule>"
}

! /^[ \t]*php3_*/ {
	print $0
}

