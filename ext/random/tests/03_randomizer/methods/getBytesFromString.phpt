--TEST--
Random: Randomizer: getBytesFromString(): Basic functionality
--FILE--
<?php

use Random\Engine;
use Random\Engine\Mt19937;
use Random\Engine\PcgOneseq128XslRr64;
use Random\Engine\Secure;
use Random\Engine\Test\TestShaEngine;
use Random\Engine\Xoshiro256StarStar;
use Random\Randomizer;

require __DIR__ . "/../../engines.inc";

$engines = [];
$engines[] = new Mt19937(null, MT_RAND_MT19937);
$engines[] = new Mt19937(null, MT_RAND_PHP);
$engines[] = new PcgOneseq128XslRr64();
$engines[] = new Xoshiro256StarStar();
$engines[] = new Secure();
$engines[] = new TestShaEngine();
$iterations = getenv("SKIP_SLOW_TESTS") ? 10 : 250;

foreach ($engines as $engine) {
    echo $engine::class, PHP_EOL;

    $randomizer = new Randomizer($engine);
    var_dump($randomizer->getBytesFromString('a', 10));
    var_dump($randomizer->getBytesFromString(str_repeat('a', 256), 5));
    var_dump($randomizer->getBytesFromString('a', 0));

    for ($i = 1; $i < $iterations; $i++) {
        $output = $randomizer->getBytesFromString(str_repeat('ab', $i), 500);

        // This check can theoretically fail with a chance of 0.5**500.
        if (!str_contains($output, 'a') || !str_contains($output, 'b')) {
            die("failure: didn't see both a and b at {$i}");
        }
    }
}

die('success');

?>
--EXPECTF--
Deprecated: Constant MT_RAND_PHP is deprecated in %s on line %d

Deprecated: The MT_RAND_PHP variant of Mt19937 is deprecated in %s on line %d
Random\Engine\Mt19937
string(10) "aaaaaaaaaa"
string(5) "aaaaa"
string(0) ""
Random\Engine\Mt19937
string(10) "aaaaaaaaaa"
string(5) "aaaaa"
string(0) ""
Random\Engine\PcgOneseq128XslRr64
string(10) "aaaaaaaaaa"
string(5) "aaaaa"
string(0) ""
Random\Engine\Xoshiro256StarStar
string(10) "aaaaaaaaaa"
string(5) "aaaaa"
string(0) ""
Random\Engine\Secure
string(10) "aaaaaaaaaa"
string(5) "aaaaa"
string(0) ""
Random\Engine\Test\TestShaEngine
string(10) "aaaaaaaaaa"
string(5) "aaaaa"
string(0) ""
success
