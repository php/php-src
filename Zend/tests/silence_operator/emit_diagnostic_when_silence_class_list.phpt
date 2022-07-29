--TEST--
Diagnostics should still be emitted if a class list is passed to @, userland
--FILE--
<?php

function test() {
    trigger_error('Diagnostic message', E_USER_NOTICE);
    return true;
}

$var = @<Exception>test();

var_dump($var);

echo "Done\n";
?>
--EXPECTF--
Notice: Diagnostic message in %s on line %d
bool(true)
Done
