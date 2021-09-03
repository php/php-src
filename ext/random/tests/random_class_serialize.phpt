--TEST--
Test Random: serialize
--FILE--
<?php

$rngs = [];
foreach (include __DIR__ . DIRECTORY_SEPARATOR . 'number_generator' . DIRECTORY_SEPARATOR . 'common.inc' as $klass => $is_clone_or_serializable) {
    if (!$is_clone_or_serializable) {
        continue;
    }
    $rngs[] = new $klass();
}

class UserRNG extends Random\NumberGenerator
{
    protected int $current = 0;

    public function generate(): int
    {
        return ++$this->current;
    }
}

$rngs[] = new UserRNG();

foreach ($rngs as $rng) {
    $instance = new Random($rng);

    $instance->getNumberGenerator()->generate();
    $clone_instance = unserialize(serialize($instance));

    if ($instance->getNumberGenerator()->generate() !== $clone_instance->getNumberGenerator()->generate()) {
        die('failure: ' . $instnace->getNumberGenerator()::class);
    }
}

die('success');
?>
--EXPECT--
success
