--TEST--
jump 07: jump into loop (backward)
--FILE--
<?php
while (0) {
	L1: echo "bug\n";
}
jump L1;
?>
--EXPECTF--
Fatal error: 'jump' into loop or switch statement is disallowed in %sjump07.php on line 5
