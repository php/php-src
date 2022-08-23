--TEST--
Random: Engine: PcgOneseq128XslRr64: The seed parameter must work as expected
--FILE--
<?php declare(strict_types = 1);

use Random\Engine\PcgOneseq128XslRr64;

echo "Random integer seed", PHP_EOL;
$engine = new PcgOneseq128XslRr64(random_int(PHP_INT_MIN, PHP_INT_MAX));
echo PHP_EOL, PHP_EOL;

echo "Random string seed", PHP_EOL;
$engine = new PcgOneseq128XslRr64(random_bytes(16));
echo PHP_EOL, PHP_EOL;

echo "Invalid data type", PHP_EOL;
try {
    $engine = new PcgOneseq128XslRr64(1.0);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
echo PHP_EOL, PHP_EOL;

echo "Invalid string seed length", PHP_EOL;
try {
    $engine = new PcgOneseq128XslRr64('foobar');
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}
echo PHP_EOL, PHP_EOL;

echo "Valid string seed", PHP_EOL;
$engine = new PcgOneseq128XslRr64(str_repeat("\x01\x02\x03\x04\x05\x06\x07\x08", 2));

var_dump($engine);

for ($i = 0; $i < 1000; $i++) {
    $engine->generate();
}

var_dump(bin2hex($engine->generate()));

?>
--EXPECT--
Random integer seed


Random string seed


Invalid data type
Random\Engine\PcgOneseq128XslRr64::__construct(): Argument #1 ($seed) must be of type string|int|null, float given


Invalid string seed length
Random\Engine\PcgOneseq128XslRr64::__construct(): Argument #1 ($seed) must be a 16 byte (128 bit) string


Valid string seed
object(Random\Engine\PcgOneseq128XslRr64)#3 (1) {
  ["__states"]=>
  array(2) {
    [0]=>
    string(16) "7afbdfd5830d8250"
    [1]=>
    string(16) "dfc50b6959b3bafc"
  }
}
string(16) "c42016cd9005ef2e"
