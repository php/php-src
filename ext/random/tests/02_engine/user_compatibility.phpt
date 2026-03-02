--TEST--
Random: Engine: Native engines can be wrapped without changing their sequence
--FILE--
<?php

use Random\Engine;
use Random\Engine\Mt19937;
use Random\Engine\PcgOneseq128XslRr64;
use Random\Engine\Test\TestWrapperEngine;
use Random\Engine\Xoshiro256StarStar;

require __DIR__ . "/../engines.inc";

$engines = [];
$engines[] = new Mt19937(1234);
$engines[] = new PcgOneseq128XslRr64(1234);
$engines[] = new Xoshiro256StarStar(1234);

foreach ($engines as $engine) {
    echo $engine::class, PHP_EOL;

    $native_engine = clone $engine;
    $user_engine = new TestWrapperEngine(clone $engine);

    for ($i = 0; $i < 10_000; $i++) {
        if ($native_engine->generate() !== $user_engine->generate()) {
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
