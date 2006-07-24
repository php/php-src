--TEST--
zip_entry_compressionmethod() function
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$zip = zip_open(dirname(__FILE__)."/test_procedural.zip");
if (!is_resource($zip)) die("Failure");
$entries = 0;
while ($entry = zip_read($zip)) {
  echo zip_entry_compressionmethod($entry)."\n";
}
zip_close($zip);

?>
--EXPECT--
stored
stored
stored
deflated

