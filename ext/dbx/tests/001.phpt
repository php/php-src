--TEST--
Check for dbx presence
--SKIPIF--
<?php if (!extension_loaded("dbx")) print("skip"); ?>
--FILE--
<?php 
print("dbx extension is available");
?>
--EXPECT--
dbx extension is available