--TEST--
labeled break 01: Old style "break <number>"
--FILE--
<?php
while (1) {
	echo "1: ";
	break 1;
	echo "bug\n";
}
echo "ok\n";

while (1) {
	echo "2: ";
	while (1) {
		break 2;
		echo "bug\n";
	}
	echo "bug\n";
}
echo "ok\n";

while (1) {
	echo "3: ";
	while (1) {
		break 1;
		echo "bug\n";
	}
	break 1;
	echo "bug\n";
}
echo "ok\n";

while (1) {
	echo "4: ";
	switch (1) {
		case 1:
			break 2;
			echo "bug\n";
	}
	echo "bug\n";
}
echo "ok\n";

while (1) {
	while (1) {
		break 3;
	}
}
?>
--EXPECTF--
1: ok
2: ok
3: ok
4: ok

Fatal error: Cannot break/continue 3 levels in %sbreak_label01.php on line %d
