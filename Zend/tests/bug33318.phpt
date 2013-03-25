--TEST--
Bug #33318 (throw 1; results in Invalid opcode 108/1/8)
--FILE--
<?php
throw 1;
?>
--EXPECTF--
Fatal error: Can only throw objects in %sbug33318.php on line 2