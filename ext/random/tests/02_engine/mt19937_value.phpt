--TEST--
Random: Engine: Mt19937: value
--FILE--
<?php

$engine = new \Random\Engine\Mt19937(1234);

for ($i = 0; $i < 10000; $i++) {
    $engine->generate();
}

echo \bin2hex($engine->generate());
?>
--EXPECT--
60fe95d9
