--TEST--
labeled break 04: Invalid label (another loop)
--FILE--
<?php
echo "bug\n";
L1: while (1) {
	break;
}
L2: while (1) {
	break L1;
}
?>
--EXPECTF--
Fatal error: break to label 'L1', that doesn't mark outer loop in %sbreak_label04.php on line %d
