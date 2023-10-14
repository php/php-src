--TEST--
Random: Engine: Serialization of user engines must preserve the sequence
--FILE--
<?php

use Random\Engine;
use Random\Engine\Test\TestCountingEngine32;
use Random\Engine\Test\TestShaEngine;

require __DIR__ . "/../engines.inc";

$engines = [];
$engines[] = new TestCountingEngine32();
$engines[] = new TestShaEngine();

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
Random\Engine\Test\TestCountingEngine32
Random\Engine\Test\TestShaEngine
success
