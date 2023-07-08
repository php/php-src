--TEST--
Random: Randomizer: Serialization of the Randomizer must preserve the sequence
--FILE--
<?php

use Random\Engine;
use Random\Engine\Mt19937;
use Random\Engine\PcgOneseq128XslRr64;
use Random\Engine\Test\TestShaEngine;
use Random\Engine\Xoshiro256StarStar;
use Random\Randomizer;

require __DIR__ . "/../engines.inc";

$engines = [];
$engines[] = new Mt19937(1234, MT_RAND_MT19937);
$engines[] = new Mt19937(1234, MT_RAND_PHP);
$engines[] = new PcgOneseq128XslRr64(1234);
$engines[] = new Xoshiro256StarStar(1234);
$engines[] = new TestShaEngine("1234");

foreach ($engines as $engine) {
    echo $engine::class, PHP_EOL;

    $randomizer = new Randomizer($engine);

    for ($i = 0; $i < 10_000; $i++) {
        $randomizer->getInt(0, $i);
    }

    $randomizer2 = unserialize(serialize($randomizer));

    for ($i = 0; $i < 10_000; $i++) {
        if ($randomizer->getInt(0, $i) !== $randomizer2->getInt(0, $i)) {
            $className = $engine::class;

            die("failure: state differs at {$i}");
        }
    }
}

die('success');
?>
--EXPECTF--
Deprecated: Constant MT_RAND_PHP is deprecated in %s on line %d

Deprecated: The MT_RAND_PHP variant of Mt19937 is deprecated in %s on line %d
Random\Engine\Mt19937
Random\Engine\Mt19937
Random\Engine\PcgOneseq128XslRr64
Random\Engine\Xoshiro256StarStar
Random\Engine\Test\TestShaEngine
success
