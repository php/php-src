--TEST--
jump 08: jump into loop (forward)
--FILE--
<?php
jump L1;
while (0) {
	L1: echo "bug\n";
}
?>
--EXPECTF--
Fatal error: 'jump' into loop or switch statement is disallowed in %sjump08.php on line 2
