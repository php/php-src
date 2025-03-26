--TEST--
resolution of outer scopes
--FILE--
<?php

class Outside {}

class Outer {
    class Inner extends Outside {}
}

echo (new Outer\Inner) instanceof Outside;
?>
--EXPECT--
1
