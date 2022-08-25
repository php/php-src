--TEST--
Random: Randomizer: getBytes(): Basic functionality
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

foreach ($engines as $engine) {
    echo $engine::class, PHP_EOL;

    $randomizer = new Randomizer($engine);

    // Using 10_000 is very slow.
    for ($i = 1; $i < 1_000; $i++) {
        if (\strlen($randomizer->getBytes($i)) !== $i) {
            die("failure: incorrect string length at {$i}");
        }
    }
}

die('success');

?>
--EXPECT--
Random\Engine\Mt19937
Random\Engine\Mt19937
Random\Engine\PcgOneseq128XslRr64
Random\Engine\Xoshiro256StarStar
Random\Engine\Secure
Random\Engine\Test\TestShaEngine
success
