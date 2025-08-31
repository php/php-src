--TEST--
Test DES with invalid fallback
--FILE--
<?php

// musl tries to support invalid DES salts unless they would violate
// the /etc/passwd format, so we include colons to ensure that musl
// crypt() will fail on these inputs as well.
var_dump(crypt("test", "$:#"));
var_dump(crypt("test", "$:5zd$01\n"));

?>
--EXPECT--
string(2) "*0"
string(2) "*0"
