--TEST--
jump 06: goto to undefined label
--FILE--
<?php
goto L1;
?>
--EXPECTF--
Fatal error: 'goto' to undefined label 'L1' in %sjump06.php on line 2
