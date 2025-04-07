--TEST--
aliases cannot escape
--FILE--
<?php

class Outer {
    private class Inner {}
}

class_alias(Outer\Inner::class, 'InnerAlias');

new InnerAlias();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot instantiate class Outer\Inner from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
