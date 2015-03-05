--TEST--
using const with same name as defined should fail
--FILE--
<?php

namespace {
    use const foo\bar;
    const bar = 42;

}

namespace {
    echo "Done";
}

?>
--EXPECTF--
Fatal error: Cannot declare const bar because the name is already in use in %s on line %d
