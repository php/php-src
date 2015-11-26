--TEST--
importing function with same name but different case should fail
--FILE--
<?php

namespace {
    use function foo\bar;
    use function foo\BAR;
}

?>
--EXPECTF--
Fatal error: Cannot use function foo\BAR as BAR because the name is already in use in %s on line %d
