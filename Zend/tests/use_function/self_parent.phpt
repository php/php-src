--TEST--
Allow self and parent in use function statement
--FILE--
<?php

namespace {
    use function self as foo;
    use function parent as bar;
}

?>
--EXPECT--
