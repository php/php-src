--TEST--
quoted_printable_encode() tests - 1
--FILE--
<?php

var_dump(quoted_printable_encode());
var_dump(quoted_printable_encode(""));
var_dump(quoted_printable_encode("test"));
var_dump(quoted_printable_encode("test", "more"));

$a = array("str");
var_dump(quoted_printable_encode($a));
var_dump(quoted_printable_encode(1));
var_dump(quoted_printable_encode(NULL));
var_dump(quoted_printable_encode(false));

echo "Done\n";
?>
--EXPECTF--	
Warning: quoted_printable_encode() expects exactly 1 parameter, 0 given in %s on line %d
NULL
string(0) ""
string(4) "test"

Warning: quoted_printable_encode() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: quoted_printable_encode() expects parameter 1 to be string, array given in %s on line %d
NULL
string(1) "1"
string(0) ""
string(0) ""
Done
