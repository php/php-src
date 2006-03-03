--TEST--
labeled break 09: Use the same <label> name in several copies of one file
--FILE--
<?php
if (isset($n)) {
	++$n;
} else {
	$n = 1;
}
L1: while (1) {	
	echo "$n: ";
	L2: while (1) {	
		break L1;
		echo "bug\n";
	}
	echo "bug\n";
}
echo "ok\n";
if ($n < 3) {
	include(__FILE__);
}
?>
--EXPECTF--
1: ok
2: ok
3: ok
