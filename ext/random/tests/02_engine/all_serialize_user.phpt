--TEST--
Random: Engine: Serialization of user engines must preserve the sequence
--FILE--
<?php

use Random\Engine;
use Random\Engine\Test\TestShaEngine;

require __DIR__ . "/../engines.inc";

final class CountingEngine32 implements Engine
{
    private int $count = 0;

    public function generate(): string
    {
        return pack('V', ++$this->count);
    }
}

$engines = [];
$engines[] = new CountingEngine32();
$engines[] = new TestShaEngine();
$iterations = getenv("SKIP_SLOW_TESTS") ? 3_000 : 10_000;

foreach ($engines as $engine) {
    echo $engine::class, PHP_EOL;

    for ($i = 0; $i < 10_000; $i++) {
        $engine->generate();
    }

    $engine2 = unserialize(serialize($engine));

    for ($i = 0; $i < $iterations; $i++) {
        if ($engine->generate() !== $engine2->generate()) {
            die("failure: state differs at {$i}");
        }
    }
}

die('success');

?>
--EXPECT--
CountingEngine32
Random\Engine\Test\TestShaEngine
success
