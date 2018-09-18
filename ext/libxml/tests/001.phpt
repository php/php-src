--TEST--
libxml_use_internal_errors()
--SKIPIF--
<?php if (!extension_loaded('libxml')) die('skip'); ?>
--FILE--
<?php

var_dump(libxml_use_internal_errors(false));
var_dump(libxml_use_internal_errors(true));
var_dump(libxml_use_internal_errors());
var_dump(libxml_use_internal_errors(new stdclass));

var_dump(libxml_get_errors());
var_dump(libxml_get_last_error());

var_dump(libxml_clear_errors());

echo "Done\n";
?>
--EXPECTF--
bool(false)
bool(false)
bool(true)

Warning: libxml_use_internal_errors() expects parameter 1 to be bool, object given in %s001.php on line 6
NULL
array(0) {
}
bool(false)
NULL
Done
