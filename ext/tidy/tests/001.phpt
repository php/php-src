--TEST--
Check for tidy presence
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
echo "tidy extension is available";
?>
--EXPECT--
tidy extension is available
