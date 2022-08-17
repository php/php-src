--TEST--
Random: Engine: Xoshiro256StarStar: seed
--FILE--
<?php declare(strict_types = 1);

$engine = new \Random\Engine\Xoshiro256StarStar(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$engine = new \Random\Engine\Xoshiro256StarStar(\random_bytes(32));

try {
    $engine = new Random\Engine\Xoshiro256StarStar(1.0);
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    $engine = new Random\Engine\Xoshiro256StarStar('foobar');
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    $engine = new Random\Engine\Xoshiro256StarStar(\str_repeat("\x00", 32));
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
}

$engine = new \Random\Engine\Xoshiro256StarStar("\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");

\var_dump($engine);

for ($i = 0; $i < 1000; $i++) {
    $engine->generate();
}
\var_dump(\bin2hex($engine->generate()));

?>
--EXPECTF--
Random\Engine\Xoshiro256StarStar::__construct(): Argument #1 ($seed) must be of type string|int|null, float given
Random\Engine\Xoshiro256StarStar::__construct(): Argument #1 ($seed) must be a 32 byte (256 bit) string
Random\Engine\Xoshiro256StarStar::__construct(): Argument #1 ($seed) must not consist entirely of NUL bytes
object(Random\Engine\Xoshiro256StarStar)#%d (%d) {
  ["__states"]=>
  array(4) {
    [0]=>
    string(16) "0102030405060708"
    [1]=>
    string(16) "0102030405060708"
    [2]=>
    string(16) "0102030405060708"
    [3]=>
    string(16) "0102030405060708"
  }
}
string(16) "90a025df9300cfd1"
