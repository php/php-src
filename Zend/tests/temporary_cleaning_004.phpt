--TEST--
Temporary leak with switch
--FILE--
<?php

function ops() {
    throw new Exception();
}

$a = [new StdClass, new StdClass];
switch ($a[0]) {
	case false:
	break;
	default:
		try {
			$x = 2;
			$y = new StdClass;
			while ($x-- && new StdClass) {
				$r = [$x] + ($y ? ((array) $x) + [2] : ops());
				$y = (array) $y;
			}
		} catch (Exception $e) {
		}
}

try {
	switch ($a[0]) {
		case false:
		break;
		default:
			$x = 2;
			$y = new StdClass;
			while ($x-- && new StdClass) {
				$r = [$x] + ($y ? ((array) $x) + [2] : ops());
				$y = (array) $y;
			}
	}
} catch (Exception $e) {
}

?>
==DONE==
--EXPECT--
==DONE==
