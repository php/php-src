--TEST--
bug 64441, FILTER_VALIDATE_URL will invalidate a hostname that ended by dot
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
var_dump(filter_var('http://example.com./', FILTER_VALIDATE_URL));
var_dump(filter_var('http://example.com/', FILTER_VALIDATE_URL));
?>
--EXPECT--
string(20) "http://example.com./"
string(19) "http://example.com/"
