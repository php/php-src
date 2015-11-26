--TEST--
use const statements with conflicting names
--FILE--
<?php

namespace foo {
    const baz = 42;
}

namespace bar {
    const baz = 42;
}

namespace {
    use const foo\baz, bar\baz;
    echo "Done\n";
}

?>
--EXPECTF--
Fatal error: Cannot use const bar\baz as baz because the name is already in use in %s on line %d
