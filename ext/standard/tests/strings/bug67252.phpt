--TEST--
Bug #67252 (convert_uudecode out-of-bounds read)
--FILE--
<?php

$a = "M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A"."\n"."a.";
var_dump(convert_uudecode($a));

?>
--EXPECTF--
Warning: convert_uudecode(): Argument #1 ($data) is not a valid uuencoded string in %s on line %d
bool(false)
