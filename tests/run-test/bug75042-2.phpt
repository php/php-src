--TEST--
phpt EXTENSIONS directive with static module
--EXTENSIONS--
SPL
--FILE--
<?php
var_dump(extension_loaded('spl'));
?>
--EXPECT--
bool(true)
