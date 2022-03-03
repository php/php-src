--TEST--
Bug #81268 Wrong message when using null as a default value for intersection types
--FILE--
<?php

class Test {
    public X&Y $y = null;
}

?>
--EXPECTF--
Fatal error: Cannot use null as default value for property Test::$y of type X&Y in %s on line %d
