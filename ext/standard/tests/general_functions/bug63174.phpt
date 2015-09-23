--TEST--
Random number generators should not overflow
--FILE--
<?php

// the biggest range we can accommodate, based on RAND_SCALE scaling algo
// 32bit: $upper == getrandmax()       // since we use 32-bit MT algo
// 64bit: getrandmax() < PHP_INT_MAX   // since we allow scaling into 64-bit range
if (! defined('PHP_INT_MIN')) {
    define('PHP_INT_MIN', (-PHP_INT_MAX-1));
}
$lower = PHP_INT_MIN;
$upper = PHP_INT_MAX;

// define our test ranges
$ranges = array (
    // nowhere close to overflowing
    array (0, 242),
    array (1, 242),
    array (65, 90),
    array (-242, -1),
    array (-242, 0),
    array (-242, 1),
    array (-10000, 10000),
    array (19781017, 20130513),
    array (-19561018, -19551124),

    // teetering on the edge of overflow
    array (1, $upper),    // barely inside range
    array (1, $upper-1),  // also barely inside range
    array (0, $upper),    // on the borders of our int size
    array ($lower, -1),   // also on the borders of our int size

    // range (max - min) exceeds PHP_INT_MAX
    // notice these all cross 0
    array (-1, $upper, true),               // one too big
    array ($lower, 1, true),                // also one too big
    array ($lower, $upper, true),           // $upper - $lower > PHP_INT_MAX
    array (-$upper,$upper, true),           // double your pleasure
    array ($lower, -($lower+1), true),      // double your pleasure, again
    array (PHP_INT_MIN, PHP_INT_MAX, true), // whoa, as big as it gets
);

foreach (array ('mt_rand', 'rand') as $prng) {
    var_dump($prng);

    // check that we produce random numbers in the given ranges
    var_dump(count($ranges));
    foreach ($ranges as $k => $range) {
        $number = mt_rand($range[0], $range[1]);
        var_dump($number);                                      // expect: integer
        var_dump($range[0] <= $number && $number <= $range[1]); // expect: true
    }

    // quick check the uniformity of the distribution
    // "each nonrandom sequence is nonrandom in its own way."
    // Observed that zero popped up a lot when there was overflow
    $n = 10000;
    $t =  4000; // 40% is well more than would be expected in a uniform distribution
    foreach ($ranges as $range) {
        if (empty($range[2])) {
            // not an overflow test, skip it
            continue;
        }
        $zeroCnt = 0;
        for ($i = 0; $i < $n; $i++) {
            $point = mt_rand($range[0], $range[1]);
            if (0 === $point) {
                $zeroCnt++;
            }
        }
        if ($t < $zeroCnt) {
            var_dump("Zero popped up more than 40% of the time in range [${range[0]}, ${range[1]})");
        }
    }
}

--EXPECTF--
string(7) "mt_rand"
int(19)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
string(4) "rand"
int(19)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
int(%i)
bool(true)
