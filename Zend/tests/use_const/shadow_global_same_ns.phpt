--TEST--
shadowing global constants defined in the same namespace as use
--FILE--
<?php

namespace foo {
    const bar = 'local';
}

namespace {
    const bar = 'global';

    use const foo\bar;
    var_dump(bar);
    echo "Done\n";
}

?>
--EXPECT--
string(5) "local"
Done
