--TEST--
NULL argument is allowed where binary string expected
--FILE--
<?php

var_dump(urldecode(null));
var_dump(urldecode(b""));
var_dump(urldecode(""));
var_dump(urldecode(array()));
var_dump(urldecode(1));
var_dump(urldecode(false));
var_dump(urldecode(new stdclass));

$fp = fopen(__FILE__, "r");
var_dump(urldecode($fp));

echo "Done\n";
?>
--EXPECTF--	
string(0) ""
string(0) ""
string(0) ""

Warning: urldecode() expects parameter 1 to be string, array given in %s on line %d
NULL
string(1) "1"
string(0) ""

Warning: urldecode() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: urldecode() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done
--UEXPECTF--
string(0) ""
string(0) ""

Warning: urldecode() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL

Warning: urldecode() expects parameter 1 to be binary string, array given in %s on line %d
NULL
string(1) "1"
string(0) ""

Warning: urldecode() expects parameter 1 to be binary string, object given in %s on line %d
NULL

Warning: urldecode() expects parameter 1 to be binary string, resource given in %s on line %d
NULL
Done
