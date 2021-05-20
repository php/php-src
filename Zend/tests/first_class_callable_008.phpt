--TEST--
First Class Callable from NEW
--FILE--
<?php
class Foo {

}

new Foo(...);
?>
--EXPECTF--
Fatal error: cannot create Closure from call to constructor in %s on line 6
