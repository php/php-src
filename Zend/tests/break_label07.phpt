--TEST--
labeled break 07: Label redefinition 
--FILE--
<?php
echo "bug\n";
L1: while (1) {	
	L2: while (1) {	
		break L1;
	}
}
L3: while (1) {	
	L1: while (1) {	
		break L3;
	}
}
--EXPECTF--
Fatal error: Label 'L1' already defined in %sbreak_label07.php on line %d
