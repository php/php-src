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
string(90) "SplFileObject::__construct(): Argument #2 ($open_mode) must be of type string, array given"
