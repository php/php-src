--TEST--
use function statements with conflicting names
--FILE--
<?php

namespace foo {
    function baz() {
        return 'foo.baz';
    }
}

namespace bar {
    function baz() {
        return 'bar.baz';
    }
}

namespace {
    use function foo\baz, bar\baz;
    echo "Done\n";
}

?>
--EXPECTF--
Fatal error: Cannot import function bar\baz as baz, baz has already been declared in %s on line %d
