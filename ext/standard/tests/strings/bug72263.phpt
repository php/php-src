--TEST--
Bug #72263 (base64_decode skips a character after padding in strict mode)
--FILE--
<?php
var_dump(base64_decode("*", true));
var_dump(base64_decode("=*", true));
var_dump(base64_decode("VVV=", true));
var_dump(base64_decode("VVV=*", true));
?>
--EXPECT--
bool(false)
bool(false)
string(2) "UU"
bool(false)
