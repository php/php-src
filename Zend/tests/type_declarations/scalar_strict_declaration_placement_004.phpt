--TEST--
Test strict declaration being first operation only 004
--FILE--
<?php
declare(strict_types=1);

namespace Foo {
    function add1(int $arg): int {
        return $arg + 1;
    }
}

namespace {
    var_dump(Foo\add1(123));
}
?>
--EXPECT--
int(124)
