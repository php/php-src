--TEST--
shadowing null with a local version
--FILE--
<?php

namespace foo {
    const null = 42;
}

?>
--EXPECTF--
Fatal error: Cannot redeclare constant 'null' in %s on line %d
