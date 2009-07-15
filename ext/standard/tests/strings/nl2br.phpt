--TEST--
nl2br() function
--FILE--
<?php
	var_dump(nl2br("test"));
	var_dump(nl2br(""));
	var_dump(nl2br(NULL));
	var_dump(nl2br("\r\n"));
	var_dump(nl2br("\n"));
	var_dump(nl2br("\r"));
	var_dump(nl2br("\n\r"));
	
	var_dump(nl2br("\n\r\r\n\r\r\r\r"));
	var_dump(nl2br("\n\r\n\n\r\n\r\r\n\r\n"));
	var_dump(nl2br("\n\r\n\n\n\n\r\r\r\r\n\r"));
	
?>
--EXPECT--
unicode(4) "test"
unicode(0) ""
unicode(0) ""
unicode(8) "<br />
"
unicode(7) "<br />
"
unicode(7) "<br />"
unicode(8) "<br />
"
unicode(44) "<br />
<br />
<br /><br /><br /><br />"
unicode(47) "<br />
<br />
<br />
<br />
<br />
<br />
"
unicode(66) "<br />
<br />
<br />
<br />
<br />
<br /><br /><br />
<br />"
