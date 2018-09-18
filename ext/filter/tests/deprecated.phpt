--TEST--
FILTER_FLAG_SCHEME_REQUIRED and FILTER_FLAG_HOST_REQUIRED are deprecated
--SKIPIF--
<?php
if (!extension_loaded('filter')) die('skip filter extension not available');
?>
--FILE--
<?php
var_dump(filter_var('//example.com/', FILTER_VALIDATE_URL, FILTER_FLAG_SCHEME_REQUIRED));
var_dump(filter_var('http://', FILTER_VALIDATE_URL, FILTER_FLAG_HOST_REQUIRED));
?>
===DONE===
--EXPECTF--
Deprecated: filter_var(): explicit use of FILTER_FLAG_SCHEME_REQUIRED and FILTER_FLAG_HOST_REQUIRED is deprecated in %s
bool(false)

Deprecated: filter_var(): explicit use of FILTER_FLAG_SCHEME_REQUIRED and FILTER_FLAG_HOST_REQUIRED is deprecated in %s
bool(false)
===DONE===
