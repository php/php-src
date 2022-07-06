--TEST--
aliasing imported constants to resolve naming conflicts
--FILE--
<?php

namespace foo {
    const baz = 42;
}

namespace bar {
    const baz = 43;
}

namespace {
    use const foo\baz as foo_baz,
              bar\baz as bar_baz;
    var_dump(foo_baz);
    var_dump(bar_baz);
    echo "Done\n";
}

?>
--EXPECT--
int(42)
int(43)
Done
