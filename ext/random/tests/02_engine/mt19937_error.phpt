--TEST--
Random: Engine: Mt19937: Invalid $mode parameter values must be rejected
--FILE--
<?php

use Random\Engine\Mt19937;

try {
    new Mt19937(1234, 2);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Random\Engine\Mt19937::__construct(): Argument #2 ($mode) must be either MT_RAND_MT19937 or MT_RAND_PHP
