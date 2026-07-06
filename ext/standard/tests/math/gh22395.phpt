--TEST--
GH-22395 (base_convert outputs at most 64 characters)
--FILE--
<?php
$result = base_convert(str_repeat("1", 61), 36, 16);
var_dump(strlen($result));
var_dump(substr($result, 0, 13));
?>
--EXPECT--
int(78)
string(13) "4b61b5e0639ff"
