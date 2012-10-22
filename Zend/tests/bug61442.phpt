--TEST--
Bug #61442 (exception threw in __autoload can not be catched)
--FILE--
<?php
function __autoload($name) {
    throw new Exception("Unable to load $name");
}

try {
    $obj = new NonLoadableClass();
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $obj = NonLoadableClass::a();
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $obj = NonLoadableClass::UNDEFINED_CONST;
} catch (Exception $e) {
    var_dump($e->getMessage());
}

--EXPECTF--
string(31) "Unable to load NonLoadableClass"
string(31) "Unable to load NonLoadableClass"
string(31) "Unable to load NonLoadableClass"
