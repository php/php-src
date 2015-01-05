--TEST--
jump 08: goto into loop (forward)
--FILE--
<?php
goto L1;
while (0) {
	L1: echo "bug\n";
}
?>
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump08.php on line 2
