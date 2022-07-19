--TEST--
Random: Engine: PcgOneseq128XslRr64: seed
--FILE--
<?php declare(strict_types = 1);

$engine = new \Random\Engine\PcgOneseq128XslRr64(\random_int(\PHP_INT_MIN, \PHP_INT_MAX));
$engine = new \Random\Engine\PcgOneseq128XslRr64(\random_bytes(16));

try {
    $engine = new Random\Engine\PcgOneseq128XslRr64(1.0);
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    $engine = new Random\Engine\PcgOneseq128XslRr64('foobar');
} catch (\Throwable $e) {
    echo $e->getMessage() . PHP_EOL;
}

$engine = new \Random\Engine\PcgOneseq128XslRr64("\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");

\var_dump($engine);

for ($i = 0; $i < 1000; $i++) {
    $engine->generate();
}
\var_dump(\bin2hex($engine->generate()));

?>
--EXPECTF--
Random\Engine\PcgOneseq128XslRr64::__construct(): Argument #1 ($seed) must be of type string|int|null, float given
Random\Engine\PcgOneseq128XslRr64::__construct(): Argument #1 ($seed) state strings must be 16 bytes
object(Random\Engine\PcgOneseq128XslRr64)#%d (%d) {
  ["__states"]=>
  array(2) {
    [0]=>
    string(16) "7afbdfd5830d8250"
    [1]=>
    string(16) "dfc50b6959b3bafc"
  }
}
string(16) "c42016cd9005ef2e"
