--TEST--
Bug #54292 (Wrong parameter causes crash in SplFileObject::__construct())
--FILE--
<?php

try {
    new SplFileObject('foo', array());
} catch (TypeError $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(95) "SplFileObject::__construct() expects argument #2 ($open_mode) to be of type string, array given"
