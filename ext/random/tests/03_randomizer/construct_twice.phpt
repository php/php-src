--TEST--
Random: Randomizer: Calling __construct() fails due to readonly $engine property
--FILE--
<?php

use Random\Engine;
use Random\Engine\PcgOneseq128XslRr64;
use Random\Engine\Test\TestShaEngine;
use Random\Engine\Xoshiro256StarStar;
use Random\Randomizer;

require __DIR__ . "/../engines.inc";

try {
    (new Randomizer())->__construct();
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    $randomizer = new Randomizer(new Xoshiro256StarStar());
    $randomizer->__construct(new PcgOneseq128XslRr64());
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    $randomizer = new Randomizer(new TestShaEngine("1234"));
    $randomizer->__construct(new TestShaEngine("1234"));
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    $randomizer = new Randomizer(new Xoshiro256StarStar());
    $randomizer->__construct(new TestShaEngine("1234"));
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

var_dump($randomizer->engine::class);

die('success');

?>
--EXPECT--
Cannot modify readonly property Random\Randomizer::$engine
Cannot modify readonly property Random\Randomizer::$engine
Cannot modify readonly property Random\Randomizer::$engine
Cannot modify readonly property Random\Randomizer::$engine
string(32) "Random\Engine\Xoshiro256StarStar"
success
