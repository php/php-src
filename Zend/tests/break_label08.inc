<?php
L1: while (1) {	
	echo "2: ";
	L2: while (1) {	
		break L1;
		echo "bug\n";
	}
	echo "bug\n";
}
echo "ok\n";
?>
