--TEST--
Random: Randomizer: readonly engine
--FILE--
<?php

$one = new \Random\Randomizer(
    new \Random\Engine\PcgOneseq128XslRr64(1234)
);

$one_ng_clone = clone $one->engine;
if ($one->engine->generate() !== $one_ng_clone->generate()) {
    die('invalid result');
}

try {
    $one->engine = $one_ng_clone;
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
}

$two = new \Random\Randomizer(
    new \Random\Engine\Secure()
);

try {
    $two_ng_clone = clone $two->engine;
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    $two->engine = $one_ng_clone;
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
}

die('success')
?>
--EXPECT--
Cannot modify readonly property Random\Randomizer::$engine
Trying to clone an uncloneable object of class Random\Engine\Secure
Cannot modify readonly property Random\Randomizer::$engine
success
