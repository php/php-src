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

Warning: urldecode() expects parameter 1 to be string (Unicode or binary), array given in /home/dmitry/php/php6/Zend/tests/null_argument_001.php on line 6
NULL
string(1) "1"
string(0) ""

Warning: urldecode() expects parameter 1 to be string (Unicode or binary), object given in /home/dmitry/php/php6/Zend/tests/null_argument_001.php on line 9
NULL

Warning: urldecode() expects parameter 1 to be string (Unicode or binary), resource given in /home/dmitry/php/php6/Zend/tests/null_argument_001.php on line 12
NULL
Done
