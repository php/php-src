--TEST--
Test hrtime() aligns with microtime()
--FLAKY--
This test frequently fails in CI
--FILE--
<?php

/* Busy-wait for a fixed amount of wall clock time instead of for a fixed
 * number of iterations. The relative uncertainty checked below is dominated
 * by scheduling jitter between the microtime() and hrtime() calls, so it is
 * only meaningful once the measured interval is long enough to dwarf it. */
$m0 = microtime(true);
$h0 = hrtime(true);
do {
    $h1 = hrtime(true);
} while ($h1 - $h0 < 100000000); /* 100ms */
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
