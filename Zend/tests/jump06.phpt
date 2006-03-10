--TEST--
jump 06: jump to undefined label
--FILE--
<?php
jump L1;
?>
--EXPECTF--
Fatal error: 'jump' to undefined label 'L1' in %sjump06.php on line 2
