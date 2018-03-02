--TEST--
Check for tidy presence
--SKIPIF--
<?php if(!extension_loaded("tidy")) die("skip tidy extension not loaded"); ?>
--FILE--
<?php 
echo "tidy extension is available";
?>
--EXPECT--
tidy extension is available
