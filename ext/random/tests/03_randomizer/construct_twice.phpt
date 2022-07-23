--TEST--
Random: Randomizer: Disallow manually calling __construct
--FILE--
<?php

final class UserEngine implements \Random\Engine
{
    public function generate(): string
    {
        return \random_byte(4); /* 32-bit */
    }
}

try {
    (new \Random\Randomizer())->__construct();
} catch (\BadMethodCallException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    $r = new \Random\Randomizer(new \Random\Engine\Xoshiro256StarStar());
    $r->__construct(new \Random\Engine\PcgOneseq128XslRr64());
} catch (\BadMethodCallException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    $r = new \Random\Randomizer(new \UserEngine());
    $r->__construct(new \UserEngine());
} catch (\BadMethodCallException $e) {
    echo $e->getMessage() . PHP_EOL;
}

die('success');
?>
--EXPECT--
Cannot call constructor twice
Cannot call constructor twice
Cannot call constructor twice
success
