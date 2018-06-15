--TEST--
Check for zip presence
--SKIPIF--
<?php if(!extension_loaded("zip")) die("skip zip extension not loaded"); ?>
--FILE--
<?php
echo "zip extension is available";
?>
--EXPECT--
zip extension is available
