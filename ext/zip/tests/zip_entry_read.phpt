--TEST--
zip_entry_read() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$zip    = zip_open(__DIR__."/test_procedural.zip");
$entry  = zip_read($zip);
if (!zip_entry_open($zip, $entry, "r")) die("Failure");
echo zip_entry_read($entry);
zip_entry_close($entry);
zip_close($zip);

?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated in %s on line %d

Deprecated: Function zip_read() is deprecated in %s on line %d

Deprecated: Function zip_entry_open() is deprecated in %s on line %d

Deprecated: Function zip_entry_read() is deprecated in %s on line %d
foo


Deprecated: Function zip_entry_close() is deprecated in %s on line %d

Deprecated: Function zip_close() is deprecated in %s on line %d
