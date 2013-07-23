--TEST--
shadowing a global constant with a local version
--FILE--
<?php

namespace {
    const bar = 'global bar';
}

namespace foo {
    const bar = 'local bar';
}

namespace {
    var_dump(bar);
}

namespace {
    use const foo\bar;
    var_dump(bar);
    echo "Done\n";
}

?>
--EXPECT--
string(10) "global bar"
string(9) "local bar"
Done
