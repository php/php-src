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
Fatal error: Cannot import function foo\BAR as BAR, BAR has already been declared in %s on line %d
