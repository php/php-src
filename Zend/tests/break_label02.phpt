--TEST--
labeled break 02: New style "break <label>"
--FILE--
<?php
L1: while (1) {
	echo "1: ";
	break L1;
	echo "bug\n";
}

echo "ok\n";

L2: while (1) {
	echo "2: ";
	while (1) {
		break L2;
		echo "bug\n";
	}
	echo "bug\n";
}

echo "ok\n";

L3: while (1) {
	echo "3: ";
	L4: while (1) {
		break L4;
		echo "bug\n";
	}
	break L3;
	echo "bug\n";
}

echo "ok\n";

L5: while (1) {
	echo "4: ";
	switch (1) {
		case 1:
			break L5;
			echo "bug\n";
	}
	echo "bug\n";
}

echo "ok\n";

L6: while (1) {
	echo "5: ";
	L7: switch (1) {
		case 1:
			break L7;
			echo "bug\n";
	}
	break L6;
	echo "bug\n";
}

echo "ok\n";
?>
--EXPECT--
1: ok
2: ok
3: ok
4: ok
5: ok
