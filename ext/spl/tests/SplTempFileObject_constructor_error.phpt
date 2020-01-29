--TEST--
SPL SplTempFileObject constructor sets correct defaults when pass 0 arguments
--FILE--
<?php
try {
    new SplTempFileObject('invalid');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
SplTempFileObject::__construct() expects argument #1 ($max_memory) to be of type int, string given
