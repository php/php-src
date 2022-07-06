--TEST--
zip_entry_filesize() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$zip = zip_open(__DIR__."/test_procedural.zip");
if (!is_resource($zip)) die("Failure");
$entries = 0;
while ($entry = zip_read($zip)) {
  echo zip_entry_filesize($entry)."\n";
}
zip_close($zip);

?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated in %s on line %d

Deprecated: Function zip_read() is deprecated in %s on line %d

Deprecated: Function zip_entry_filesize() is deprecated in %s on line %d
5

Deprecated: Function zip_read() is deprecated in %s on line %d

Deprecated: Function zip_entry_filesize() is deprecated in %s on line %d
4

Deprecated: Function zip_read() is deprecated in %s on line %d

Deprecated: Function zip_entry_filesize() is deprecated in %s on line %d
0

Deprecated: Function zip_read() is deprecated in %s on line %d

Deprecated: Function zip_entry_filesize() is deprecated in %s on line %d
27

Deprecated: Function zip_read() is deprecated in %s on line %d

Deprecated: Function zip_close() is deprecated in %s on line %d
