--TEST--
Bug #33076 (str_ireplace() incorrectly counts result string length and may cause segfault)
--FILE--
<?php

$value = str_ireplace("t", "bz", "Text");

var_dump($value);

echo "Done\n";
?>
--EXPECT--	
string(6) "bzexbz"
Done
