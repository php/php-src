--TEST--
Bug #70557 (Memleak on return type verifying failed).
--FILE--
<?php

function getNumber() : int {
    return "foo";
}

try {
    getNumber();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: getNumber(): Return value must be of type int, string returned
