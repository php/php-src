--TEST--
use and use const with the same alias
--FILE--
<?php

namespace {
    const foo = 'foo';
}

namespace x {
    use foo as bar;
    use const foo as bar;
    var_dump(bar);
}

?>
--EXPECT--
string(3) "foo"
