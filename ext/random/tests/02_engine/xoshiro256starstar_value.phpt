--TEST--
Random: Engine: Xoshiro256StarStar: For a reference seed a fixed reference value must be generated
--FILE--
<?php

use Random\Engine\Xoshiro256StarStar;

$engine = new Xoshiro256StarStar(1234);

for ($i = 0; $i < 10_000; $i++) {
    $engine->generate();
}

$engine->jump();
$engine->jumpLong();

var_dump(bin2hex($engine->generate()));

?>
--EXPECT--
string(16) "1f197e9ca7969123"
