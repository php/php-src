--TEST--
Test Random: NumberGenerator: inheritance
--FILE--
<?php

$rngs = [];
foreach (include __DIR__ . DIRECTORY_SEPARATOR . 'common.inc' as $klass => $is_clone_or_serializable) {
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
    if (!$rng instanceof Random\NumberGenerator\RandomNumberGenerator) {
        die('failure: ' . $rng::class);
    }
}

die('success');
?>
--EXPECT--
success
