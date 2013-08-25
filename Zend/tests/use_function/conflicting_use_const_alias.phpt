--TEST--
use const and use function with the same alias
--FILE--
<?php

namespace {
    const foo = 'foo.const';
    function foo() {
        return 'foo.function';
    }
}

namespace x {
    use const foo as bar;
    use function foo as bar;
    var_dump(bar);
    var_dump(bar());
}

?>
--EXPECT--
string(9) "foo.const"
string(12) "foo.function"
