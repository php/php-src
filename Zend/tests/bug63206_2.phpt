--TEST--
Bug #63206 Fully support exception_handler stacking, even with null
--FILE--
<?php

set_exception_handler(function() {
    echo 'First handler' . PHP_EOL;
});

set_exception_handler(function() {
    echo 'Second handler' . PHP_EOL;
});

set_exception_handler(null);

set_exception_handler(function() {
    echo 'Fourth handler' . PHP_EOL;
});

restore_exception_handler();
restore_exception_handler();

throw new Exception();
?>
--EXPECTF--
Second handler
