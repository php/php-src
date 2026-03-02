--TEST--
json_decode() tests
--FILE--
<?php
var_dump(json_encode('a/b'));
var_dump(json_encode('a/b', JSON_UNESCAPED_SLASHES));
?>
--EXPECT--
string(6) ""a\/b""
string(5) ""a/b""
