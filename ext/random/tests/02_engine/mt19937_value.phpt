--TEST--
Random: Engine: Mt19937: For a reference seed a fixed reference value must be generated
--FILE--
<?php

use Random\Engine\Mt19937;

$engine = new Mt19937(1234);

for ($i = 0; $i < 10_000; $i++) {
    $engine->generate();
}

var_dump(bin2hex($engine->generate()));

?>
--EXPECT--
string(8) "60fe95d9"
