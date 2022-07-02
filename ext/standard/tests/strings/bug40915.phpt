--TEST--
Bug #40915 (addcslashes unexpected behavior with binary input)
--FILE--
<?php

$str = "a\000z";

var_dump(addslashes($str));
var_dump(addcslashes($str, ""));
var_dump(addcslashes($str, "\000z"));
var_dump(addcslashes($str, "z"));

echo "Done\n";
?>
--EXPECTF--
string(4) "a\0z"
string(3) "a%0z"
string(7) "a\000\z"
string(4) "a%0\z"
Done
