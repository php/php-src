--TEST--
Bug #76735 (Incorrect message in fopen on invalid mode)
--FILE--
<?php
fopen(__FILE__, 'Q');
?>
--EXPECTF--
Warning: fopen(%s): failed to open stream: `Q' is not a valid mode for fopen in %s on line %d
