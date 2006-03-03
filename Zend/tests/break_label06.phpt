--TEST--
labeled break 06: Invalid label (non declared yet)
--FILE--
<?php
echo "bug\n";
while (1) {	
	break L1;
}
L1:
?>
--EXPECTF--
Fatal error: break to undefined label 'L1' in %sbreak_label06.php on line %d
