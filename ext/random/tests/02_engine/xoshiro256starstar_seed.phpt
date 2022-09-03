--TEST--
Random: Engine: Xoshiro256StarStar: The seed parameter must work as expected
--FILE--
<?php declare(strict_types = 1);

use Random\Engine\Xoshiro256StarStar;

echo "Random integer seed", PHP_EOL;
$engine = new Xoshiro256StarStar(random_int(PHP_INT_MIN, PHP_INT_MAX));
echo PHP_EOL, PHP_EOL;

echo "Random string seed", PHP_EOL;
$engine = new Xoshiro256StarStar(random_bytes(32));
echo PHP_EOL, PHP_EOL;

echo "Invalid data type", PHP_EOL;
try {
    $engine = new Xoshiro256StarStar(1.0);
} catch (Throwable $e) {
    echo $e->getMessage(), PHP_EOL;
}
echo PHP_EOL, PHP_EOL;

echo "Invalid string seed length", PHP_EOL;
try {
    $engine = new Xoshiro256StarStar('foobar');
} catch (Throwable $e) {
    echo $e->getMessage(), PHP_EOL;
}
echo PHP_EOL, PHP_EOL;

echo "Null seed", PHP_EOL;
try {
    $engine = new Xoshiro256StarStar(str_repeat("\x00", 32));
} catch (Throwable $e) {
    echo $e->getMessage(), PHP_EOL;
}
echo PHP_EOL, PHP_EOL;

echo "Valid string seed", PHP_EOL;
$engine = new Xoshiro256StarStar(str_repeat("\x01\x02\x03\x04\x05\x06\x07\x08", 4));

var_dump($engine);

for ($i = 0; $i < 1000; $i++) {
    $engine->generate();
}

var_dump(bin2hex($engine->generate()));

?>
--EXPECTF--
Random integer seed


Random string seed


Invalid data type
Random\Engine\Xoshiro256StarStar::__construct(): Argument #1 ($seed) must be of type string|int|null, float given


Invalid string seed length
Random\Engine\Xoshiro256StarStar::__construct(): Argument #1 ($seed) must be a 32 byte (256 bit) string


Null seed
Random\Engine\Xoshiro256StarStar::__construct(): Argument #1 ($seed) must not consist entirely of NUL bytes


Valid string seed
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
