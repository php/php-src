--TEST--
oss-fuzz #60736: Bad constant propagation in JMP_NULL
--FILE--
<?php
(1?4:y)?->y;
?>
--EXPECTF--
Warning: Attempt to read property "y" on int in %s on line %d
