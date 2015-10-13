--TEST--
'integer' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as integer;
--EXPECTF--
Fatal error: Cannot use foobar as integer because 'integer' is a special class name in %s on line %d