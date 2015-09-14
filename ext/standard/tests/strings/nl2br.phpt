--TEST--
nl2br() function
--DESCRIPTION--
This tests \nl2br() PHP core/string function and its
behavior with different arguments passed.
--FILE--
<?php
	// without any newlines
	var_dump( nl2br("test"));
	var_dump( nl2br(''));
	var_dump( nl2br(NULL));

	// CRLF
	var_dump( nl2br("\r\n"));

	// LF
	var_dump( nl2br("\n"));

	// CR
	var_dump( nl2br("\r"));

	// LFCR
	var_dump( nl2br("\n\r"));
	
	// mixins
	var_dump( nl2br("\n\r\r\n\r\r\r\r"));
	var_dump( nl2br("\n\r\n\n\r\n\r\r\n\r\n"));
	var_dump( nl2br("\n\r\n\n\n\n\r\r\r\r\n\r"));

	// second arfgument: $is_xhtml [ = true ]
	var_dump( nl2br("Some text\nwith newlines\nin it", false));
	var_dump( nl2br("Some text\nwith newlines\nin it", true));
	var_dump( nl2br("\r\n", false)); // CRLF convert to HTML
?>
--EXPECT--
string(4) "test"
string(0) ""
string(0) ""
string(8) "<br />
"
string(7) "<br />
"
string(7) "<br />"
string(8) "<br />
"
string(44) "<br />
<br />
<br /><br /><br /><br />"
string(47) "<br />
<br />
<br />
<br />
<br />
<br />
"
string(66) "<br />
<br />
<br />
<br />
<br />
<br /><br /><br />
<br />"
string(37) "Some text<br>
with newlines<br>
in it"
string(41) "Some text<br />
with newlines<br />
in it"
string(6) "<br>
"
--CREDITS--
In 2015 rewritten by Kubo2 <kelerest123@gmail.com>

