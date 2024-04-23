--TEST--
defining function with same name as imported should fail
--FILE--
<?php

namespace {
    use function foo\bar;

    function bar() {}
}

?>
--EXPECTF--
Fatal error: Cannot redeclare function bar() (previously declared as local import) in %s on line %d
