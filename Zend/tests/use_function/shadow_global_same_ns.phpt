--TEST--
shadowing global functions defined in the same namespace as use
--FILE--
<?php

namespace foo {
    function bar() {
        return 'local';
    }
}

namespace {
    function bar() {
        return 'global';
    }

    use function foo\bar;
    var_dump(bar());
    echo "Done\n";
}

?>
--EXPECT--
string(5) "local"
Done
