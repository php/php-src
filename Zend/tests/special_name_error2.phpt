--TEST--
Cannot use special class name as alias
--FILE--
<?php

use Foo\Bar as self;

?>
--EXPECTF--
Fatal error: Cannot import class Foo\Bar as "self", self is a reserved class nam in %s on line %d
