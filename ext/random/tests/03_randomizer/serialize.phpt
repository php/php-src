--TEST--
Random: Randomizer: serialize
--FILE--
<?php

$engines = [];
$engines[] = new \Random\Engine\Mt19937(\random_int(\PHP_INT_MIN, \PHP_INT_MAX), MT_RAND_MT19937);
$engines[] = new \Random\Engine\Mt19937(\random_int(\PHP_INT_MIN, \PHP_INT_MAX), MT_RAND_PHP);
$engines[] = new \Random\Engine\PcgOneseq128XslRr64(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$engines[] = new \Random\Engine\Xoshiro256StarStar(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$engines[] = new \Random\Engine\Secure(); 
$generate = \random_bytes(16);
$engines[] = new class () implements Random\Engine {
    public function generate(): string
    {
        global $generate;
        return $generate;
    }
};
class UserEngine implements Random\Engine
{
    public function generate(): string
    {
        global $generate;
        return $generate;
    }
}
$engines[] = new UserEngine();

foreach ($engines as $engine) {
    $randomizer = new Random\Randomizer($engine);
    $randomizer->getInt(\PHP_INT_MIN, \PHP_INT_MAX);
    try {
        $randomizer2 = unserialize(serialize($randomizer));
    } catch (\Exception $e) {
        echo $e->getMessage() . PHP_EOL;
        continue;
    }

    if ($randomizer->getInt(\PHP_INT_MIN, \PHP_INT_MAX) !== $randomizer2->getInt(\PHP_INT_MIN, \PHP_INT_MAX)) {
        die($engine::class . ': failure.');
    }

    echo $engine::class . ': success' . PHP_EOL;
}

die('success');
?>
--EXPECTF--
Random\Engine\Mt19937: success
Random\Engine\Mt19937: success
Random\Engine\PcgOneseq128XslRr64: success
Random\Engine\Xoshiro256StarStar: success
Serialization of 'Random\Engine\Secure' is not allowed
Serialization of 'Random\Engine@anonymous' is not allowed
UserEngine: success
success
