--TEST--
Random: Randomizer: getBytesFromString(): Fast Path Masking
--FILE--
<?php

use Random\Engine\Test\TestWrapperEngine;
use Random\Engine\Xoshiro256StarStar;
use Random\Randomizer;

require __DIR__ . "/../../engines.inc";

$allBytes = implode('', array_map(
    fn ($byte) => chr($byte),
    range(0x00, 0xff)
));

// Xoshiro256** is the fastest engine available.
$xoshiro = new Xoshiro256StarStar();

var_dump(strlen($allBytes));
echo PHP_EOL;

// Fast path: Inputs less than or equal to 256.
for ($i = 1; $i <= strlen($allBytes); $i *= 2) {
    echo "{$i}:", PHP_EOL;

    $wrapper = new TestWrapperEngine($xoshiro);
    $r = new Randomizer($wrapper);
    $result = $r->getBytesFromString(substr($allBytes, 0, $i), 20000);

    // Xoshiro256** is a 64 Bit engine and thus generates 8 bytes at once.
    // For powers of two we expect no rejections and thus exactly
    // 20000/8 = 2500 calls to the engine.
    var_dump($wrapper->getCount());

    $count = [];
    for ($j = 0; $j < strlen($result); $j++) {
        $b = $result[$j];
        $count[ord($b)] ??= 0;
        $count[ord($b)]++;
    }

    // We also expect that each possible value appears at least once, if
    // not is is very likely that some bits were erroneously masked away.
    var_dump(count($count));

    echo PHP_EOL;
}

// Test lengths that are one more than the powers of two. For these
// the maximum offset will be a power of two and thus a minimal number
// of bits will be set in the offset.
for ($i = 1; ($i + 1) <= strlen($allBytes); $i *= 2) {
    $oneMore = $i + 1;

    echo "{$oneMore}:", PHP_EOL;

    $wrapper = new TestWrapperEngine($xoshiro);
    $r = new Randomizer($wrapper);
    $result = $r->getBytesFromString(substr($allBytes, 0, $oneMore), 20000);

    $count = [];
    for ($j = 0; $j < strlen($result); $j++) {
        $b = $result[$j];
        $count[ord($b)] ??= 0;
        $count[ord($b)]++;
    }

    // We expect that each possible value appears at least once, if
    // not is is very likely that some bits were erroneously masked away.
    var_dump(count($count));

    echo PHP_EOL;
}

echo "Slow Path:", PHP_EOL;

$wrapper = new TestWrapperEngine($xoshiro);
$r = new Randomizer($wrapper);
$result = $r->getBytesFromString($allBytes . $allBytes, 20000);

// In the slow path we expect one call per byte, i.e. 20000
var_dump($wrapper->getCount());

$count = [];
for ($j = 0; $j < strlen($result); $j++) {
    $b = $result[$j];
    $count[ord($b)] ??= 0;
    $count[ord($b)]++;
}

// We also expect that each possible value appears at least once, if
// not is is very likely that some bits were erroneously masked away.
var_dump(count($count));

?>
--EXPECT--
int(256)

1:
int(2500)
int(1)

2:
int(2500)
int(2)

4:
int(2500)
int(4)

8:
int(2500)
int(8)

16:
int(2500)
int(16)

32:
int(2500)
int(32)

64:
int(2500)
int(64)

128:
int(2500)
int(128)

256:
int(2500)
int(256)

2:
int(2)

3:
int(3)

5:
int(5)

9:
int(9)

17:
int(17)

33:
int(33)

65:
int(65)

129:
int(129)

Slow Path:
int(20000)
int(256)
