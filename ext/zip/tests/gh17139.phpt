--TEST--
GH-17139 - zip_entry_name() crash
--EXTENSIONS--
zip
--FILE--
<?php
$zip = zip_open(__DIR__."/test_procedural.zip");
if (!is_resource($zip)) die("Failure");
// no need to bother looping over, the entry name should point to a dangling address from the first iteration
$zip = zip_read($zip);
var_dump(zip_entry_name($zip));
?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated in %s on line %d

Deprecated: Function zip_read() is deprecated in %s on line %d

Deprecated: Function zip_entry_name() is deprecated in %s on line %d
string(3) "foo"
