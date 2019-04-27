--TEST--
Check for zip presence
--SKIPIF--
<?php if (!extension_loaded("zip")) print "skip"; ?>
--FILE--
<?php
echo "zip extension is available";
?>
--EXPECT--
zip extension is available
