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
Fatal error: Cannot import constant bar\baz as baz, baz has already been declared in %s on line %d
