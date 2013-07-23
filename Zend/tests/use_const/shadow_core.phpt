--TEST--
shadowing a global core constant with a local version
--FILE--
<?php

namespace foo {
    const PHP_VERSION = 42;
}

namespace {
    var_dump(PHP_VERSION);
}

namespace {
    use const foo\PHP_VERSION;
    var_dump(PHP_VERSION);
    echo "Done\n";
}

?>
--EXPECTF--
string(%d) "%s"
int(42)
Done
