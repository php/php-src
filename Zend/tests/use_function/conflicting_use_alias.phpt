--TEST--
use and use function with the same alias
--FILE--
<?php

namespace {
    function foo() {
        return 'foo';
    }
}

namespace x {
    use foo as bar;
    use function foo as bar;
    var_dump(bar());
}

?>
--EXPECT--
string(3) "foo"
