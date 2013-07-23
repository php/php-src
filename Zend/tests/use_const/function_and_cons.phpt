--TEST--
use function and use const in the same block
--FILE--
<?php

namespace foo {
    const bar = 'local const';
    function bar() {
        return 'local function';
    }
}

namespace {
    use const foo\bar;
    use function foo\bar;
    var_dump(bar);
    var_dump(bar());
    echo "Done\n";
}

?>
--EXPECT--
string(11) "local const"
string(14) "local function"
Done
