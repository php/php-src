--TEST--
Generic parameter can't have same name as the class
--FILE--
<?php

class Foo<Foo> {
}

?>
--EXPECTF--
Fatal error: Generic parameter Foo has same name as class in %s on line %d
