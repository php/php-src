--TEST--
labeled break 03: Undefined label
--FILE--
<?php
echo "bug\n";
L1: while (1) {
	break L2;
}
?>
--EXPECTF--
Fatal error: break to undefined label 'L2' in %sbreak_label03.php on line %d
