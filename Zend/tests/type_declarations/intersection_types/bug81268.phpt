--TEST--
Bug #81268 Message when using null as a default value for intersection types
--FILE--
<?php

class Test {
    public X&Y $y = null;
}

?>
--EXPECTF--
Fatal error: Default value for property of type X&Y may not be null. Use the nullable type ?X&Y to allow null default value in %s on line %d
