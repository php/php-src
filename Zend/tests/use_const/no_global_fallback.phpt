--TEST--
non-existent imported constants should not be looked up in the global table
--FILE--
<?php

namespace {
    const baz = NULL;
}

namespace {
    use const foo\bar\baz;
    var_dump(baz);
}

?>
--EXPECTF--
Notice: Use of undefined constant baz - assumed 'baz' in %s on line %d
string(3) "baz"
