--TEST--
labeled break 05: Invalid label (non loop)
--FILE--
<?php
L1:
echo "bug\n";
while (1) {
	break L1;
}
?>
--EXPECTF--
Fatal error: break to label 'L1', that doesn't mark outer loop in %sbreak_label05.php on line %d
