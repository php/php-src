--TEST--
shadowing a global core function with a local version
--FILE--
<?php

namespace foo {
    function strlen($str) {
        return 4;
    }
}

namespace {
    var_dump(strlen('foo bar baz'));
}

namespace {
    use function foo\strlen;
    var_dump(strlen('foo bar baz'));
    echo "Done\n";
}

?>
--EXPECT--
int(11)
int(4)
Done
