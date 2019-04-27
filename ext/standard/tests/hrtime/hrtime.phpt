--TEST--
Test hrtime() aligns with microtime()
--FILE--
<?php

$m0 = microtime(true);
$h0 = hrtime(true);
for ($i = 0; $i < 1024*1024; $i++);
$h1 = hrtime(true);
$m1 = microtime(true);

$d0 = ($m1 - $m0)*1000000000.0;
$d1 = $h1 - $h0;

/* Relative uncertainty. */
$d = abs($d0 - $d1)/$d1;

if ($d > 0.05) {
	print "FAIL, $d";
} else {
	print "OK, $d";
}

?>
--EXPECTF--
OK, %f
