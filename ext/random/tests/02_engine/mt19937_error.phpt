--TEST--
Random: Engine: Mt19937: error pattern
--FILE--
<?php

try {
    new \Random\Engine\Mt19937(1234, 2);
} catch (\InvalidArgumentException $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
Random\Engine\Mt19937::__construct(): Argument #2 ($mode) mode must be MT_RAND_MT19937 or MT_RAND_PHP
