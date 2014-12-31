--TEST--
Allow self and parent in use const statement
--FILE--
<?php

namespace {
    use const self as foo;
    use const parent as bar;
}

?>
--EXPECT--
