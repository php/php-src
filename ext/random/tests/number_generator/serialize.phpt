--TEST--
Test Random: NumberGenerator: serialize
--FILE--
<?php

$rngs = [];
foreach (include __DIR__ . DIRECTORY_SEPARATOR . 'common.inc' as $klass => $is_clone_or_serializable) {
    if (!$is_clone_or_serializable) {
        continue;
    }

    $rngs[] = new $klass();
}

class UserRNG implements Random\NumberGenerator\RandomNumberGenerator
{
    protected int $current = 0;

    public function generate(): int
    {
        return ++$this->current;
    }
}
$rngs[] = new UserRNG();

foreach ($rngs as $rng) {
    $rng->generate();
    $clone_rng = unserialize(serialize($rng));

    if ($rng->generate() !== $clone_rng->generate()) {
        die('failure: ' . $rng::class);
    }
}

die('success');
?>
--EXPECT--
success
