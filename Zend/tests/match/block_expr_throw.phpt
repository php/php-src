--TEST--
Throwing match expression block must clean up live-vars
--FILE--
<?php

function throw_($value) {
    var_dump([new \stdClass] + match ($value) {
        1 => { throw new Exception('Exception with live var'); },
    });
}

try {
    throw_(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception with live var
