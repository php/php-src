--TEST--
bug 51192, FILTER_VALIDATE_URL will invalidate a hostname that includes '-'
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
var_dump(filter_var('http://example.com/path', FILTER_VALIDATE_URL));
var_dump(filter_var('http://exa-mple.com/path', FILTER_VALIDATE_URL));
var_dump(filter_var('http://exa_mple.com/path', FILTER_VALIDATE_URL));
?>
--EXPECT--
string(23) "http://example.com/path"
string(24) "http://exa-mple.com/path"
bool(false)
