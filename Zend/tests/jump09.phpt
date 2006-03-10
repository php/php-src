--TEST--
jump 09: jump into switch (backward)
--FILE--
<?php
switch (0) {
	case 1:
		L1: echo "bug\n";
		break;
}
jump L1;
?>
--EXPECTF--
Fatal error: 'jump' into loop or switch statement is disallowed in %sjump09.php on line 7
