--TEST--
Invalid enum backing type
--FILE--
<?php

enum Foo: Bar {}

?>
--EXPECTF--
Fatal error: Enum backing type must be int or string, Bar given in %s on line %d
