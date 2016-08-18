--TEST--
Cannot use special class name as alias
--FILE--
<?php

use Foo\Bar as self;

?>
--EXPECTF--
Fatal error: Cannot use Foo\Bar as self because 'self' is a special class name in %s on line %d
