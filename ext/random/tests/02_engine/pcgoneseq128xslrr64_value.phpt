--TEST--
Random: Engine: PcgOneseq128XslRr64: For a reference seed a fixed reference value must be generated
--FILE--
<?php

use Random\Engine\PcgOneseq128XslRr64;

$engine = new PcgOneseq128XslRr64(1234);

for ($i = 0; $i < 10_000; $i++) {
    $engine->generate();
}

$engine->jump(1234567);

var_dump(bin2hex($engine->generate()));

?>
--EXPECT--
string(16) "b88e2a0f23720a1d"
