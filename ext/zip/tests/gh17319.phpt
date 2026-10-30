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
Deprecated: Function zip_open() is deprecated since 8.0, use ZipArchive::open() instead in %s on line %d

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_entry_name() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d
bool(false)
