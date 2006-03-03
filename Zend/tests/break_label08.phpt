--TEST--
labeled break 08: Use the same <label> in different files
--FILE--
<?php
L1: while (1) {	
	echo "1: ";
	L2: while (1) {	
		break L1;
		echo "bug\n";
	}
	echo "bug\n";
}
echo "ok\n";
include(dirname(__FILE__)."/break_label08.inc");
?>
--EXPECT--
1: ok
2: ok
