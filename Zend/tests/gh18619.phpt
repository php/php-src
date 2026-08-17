--TEST--
Gh-18619: exit() from error handler should not trigger bailout
--FILE--
<?php

function foo($e) {
    exit;
}

set_exception_handler('foo');

register_shutdown_function(function () {
    var_dump(set_exception_handler(null));
});

throw new Error();

?>
--EXPECT--
string(3) "foo"
