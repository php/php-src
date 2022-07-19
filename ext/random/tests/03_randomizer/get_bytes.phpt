--TEST--
Random: Randomizer: getBytes
--FILE--
<?php

$randomizer = new \Random\Randomizer (
    new class () implements \Random\Engine
    {
        private int $count = 0;

        public function generate(): string
        {
            if ($this->count > 5) {
                die('overflow');
            }

            return match ($this->count++) {
                0 => 'H',
                1 => 'e',
                2 => 'll',
                3 => 'o',
                4 => 'abcdefghijklmnopqrstuvwxyz', // 208 bits
                5 => 'success',
                default => \random_bytes(16),
            };
        }
    }
);

echo $randomizer->getBytes(5) . PHP_EOL; // Hello

echo $randomizer->getBytes(8) . PHP_EOL; // abcdefgh (64 bits)

die($randomizer->getBytes(7));

?>
--EXPECTF--
Hello
abcdefgh
success
