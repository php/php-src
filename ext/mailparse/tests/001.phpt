--TEST--
Check for mailparse presence
--SKIPIF--
<?php if (!extension_loaded("mailparse")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
echo "mailparse extension is available";
?>
--EXPECT--
mailparse extension is available
