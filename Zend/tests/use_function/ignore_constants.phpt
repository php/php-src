--TEST--
use function should ignore namespaced constants
--FILE--
<?php

namespace foo {
    const bar = 42;
}

namespace {
    const bar = 43;
}

namespace {
    use function foo\bar;
    var_dump(bar);
    echo "Done\n";
}

?>
--EXPECT--
int(43)
Done
