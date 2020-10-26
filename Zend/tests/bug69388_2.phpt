--TEST--
Bug #69388 - Variation
--FILE--
<?php
function handle_error($code, $message, $file, $line) {
    eval('namespace Foo;');
    echo "$message\n";
}

set_error_handler('handle_error');
eval('namespace {use Exception;}');

?>
--EXPECT--
Importing the global symbol Exception has no effect in the global scope
