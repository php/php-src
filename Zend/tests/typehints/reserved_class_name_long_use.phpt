--TEST--
'long' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as long;
--EXPECTF--
Fatal error: Cannot use foobar as long because 'long' is a special class name in %s on line %d