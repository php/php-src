--TEST--
jump 10: jump into switch (forward)
--FILE--
<?php
jump L1;
switch (0) {
	case 1:
		L1: echo "bug\n";
		break;
}
?>
--EXPECTF--
Fatal error: 'jump' into loop or switch statement is disallowed in %sjump10.php on line 2
