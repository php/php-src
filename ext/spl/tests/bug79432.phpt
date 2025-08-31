--TEST--
Bug #79432 (spl_autoload_call() with non-string argument violates assertion)
--FILE--
<?php

try {
    spl_autoload_call([]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
spl_autoload_call(): Argument #1 ($class) must be of type string, array given
