--TEST--
Bug #70557 (Memleak on return type verifying failed).
--FILE--
<?php

function getNumber() : int {
    return "foo";
}

try {
    getNumber();
} catch (TypeError $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(62) "getNumber(): Return value must be of type int, string returned"
