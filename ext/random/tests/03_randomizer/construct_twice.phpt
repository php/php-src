--TEST--
Random: Randomizer: Disallow manually calling __construct
--FILE--
<?php
try {
    (new \Random\Randomizer())->__construct();
} catch (\BadMethodCallException $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    (new \Random\Randomizer(
        new class () implements \Random\Engine {
            public function generate(): string
            {
                return \random_bytes(4); /* 32-bit */
            }
         }
    ))->__construct(
        new class () implements \Random\Engine {
            public function generate(): string
            {
                return \random_bytes(4); /* 32-bit */
            }
        }
    );
} catch (\BadMethodCallException $e) {
    echo $e->getMessage() . PHP_EOL;
}

die('success');
?>
--EXPECT--
Cannot call constructor twice
Cannot call constructor twice
success
