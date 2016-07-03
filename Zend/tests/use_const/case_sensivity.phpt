--TEST--
importing const with same name but different case
--FILE--
<?php

namespace {
    use const foo\bar;
    use const foo\BAR;
}

?>
--EXPECT--
