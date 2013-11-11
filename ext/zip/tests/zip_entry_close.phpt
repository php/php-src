--TEST--
zip_entry_close() function: simple and double call
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$zip    = zip_open(dirname(__FILE__)."/test_procedural.zip");
$entry  = zip_read($zip);
echo "entry_open:  "; var_dump(zip_entry_open($zip, $entry, "r"));
echo "entry_close: "; var_dump(zip_entry_close($entry));
echo "entry_close: "; var_dump(zip_entry_close($entry));
zip_close($zip);
?>
Done
--EXPECTF--
entry_open:  bool(true)
entry_close: bool(true)
entry_close: 
Warning: zip_entry_close(): %d is not a valid Zip Entry resource in %s
bool(false)
Done
