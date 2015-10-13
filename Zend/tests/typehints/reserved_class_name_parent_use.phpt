--TEST--
'parent' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as parent;
--EXPECTF--
Fatal error: Cannot use foobar as parent because 'parent' is a special class name in %s on line %d