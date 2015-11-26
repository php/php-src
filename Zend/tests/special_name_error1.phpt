--TEST--
Cannot use special class name as namespace
--FILE--
<?php

namespace self;

?>
--EXPECTF--
Fatal error: Cannot use 'self' as namespace name in %s on line %d
