--TEST--
Bug #31442 (unserialize broken on 64-bit systems)
--INI--
precision=14
--FILE--
<?php
echo unserialize(serialize(2147483648));
?>
--EXPECT--
2147483648
