--TEST--
Random: Engine: Serialization of native engines must preserve the sequence
--FILE--
<?php

use Random\Engine\Mt19937;
use Random\Engine\PcgOneseq128XslRr64;
use Random\Engine\Xoshiro256StarStar;

$engines = [];
$engines[] = new Mt19937(1234);
$engines[] = new PcgOneseq128XslRr64(1234);
$engines[] = new Xoshiro256StarStar(1234);

foreach ($engines as $engine) {
    echo $engine::class, PHP_EOL;

    for ($i = 0; $i < 10_000; $i++) {
        $engine->generate();
    }

    $engine2 = unserialize(serialize($engine));

    for ($i = 0; $i < 10_000; $i++) {
        if ($engine->generate() !== $engine2->generate()) {
            die("failure: state differs at {$i}");
        }
    }
}

die('success');

?>
--EXPECT--
Random\Engine\Mt19937
Random\Engine\PcgOneseq128XslRr64
Random\Engine\Xoshiro256StarStar
success
