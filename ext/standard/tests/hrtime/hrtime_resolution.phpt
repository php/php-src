--TEST--
Test hrtime() reduced resolution
--INI--
hrtime.resolution=20000
--FILE--
<?php

/* This may fail on a very slow machine. Two subsequent timestamp 
	probes shoul lay in the configured interval. */

$d0 = hrtime(true) - hrtime(true);
$d0 = abs($d0);

$t0 = hrtime();
$t1 = hrtime();
$d1 = ($t1[0] - $t0[0]) * 1000000000 + $t1[1] - $t0[1];

if (0 == $d0 || 20000 == $d0) {
	echo "PASS hrtime(true)\n";
} else {
	echo "Two subsequent hrtime(true) calls gave $d0\n";
}

if (0 == $d1 || 20000 == $d1) {
	echo "PASS hrtime()\n";
} else {
	echo "Two subsequent hrtime() calls gave $d1\n";
}

?>
--EXPECT--
PASS hrtime(true)
PASS hrtime()

