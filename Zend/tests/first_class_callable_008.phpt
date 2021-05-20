--TEST--
First Class Callable from NEW
--FILE--
<?php
class Foo {

}

new Foo(...);
?>
--EXPECTF--
Fatal error: cannot create Closure for new expression in %s on line 6
