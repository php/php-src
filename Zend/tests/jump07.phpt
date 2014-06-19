--TEST--
jump 07: goto into loop (backward)
--FILE--
<?php
while (0) {
	L1: echo "bug\n";
}
goto L1;
?>
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump07.php on line 5
