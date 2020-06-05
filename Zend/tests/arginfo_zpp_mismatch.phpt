--TEST--
Test that there is no arginfo/zpp mismatch
--FILE--
<?php

set_error_handler(
    function (string $code, string $message) {
        return true;
    },
    E_NOTICE | E_WARNING
);

foreach (get_defined_functions()["internal"] as $function) {
    try {
        $function(null, null, null, null, null, null, null, null);
    } catch (ArgumentCountError|Error) {
    }
}

?>
--EXPECT--
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
