--TEST--
Final private constructors cannot be overridden
--FILE--
<?php

class Base
{
    private final function __construct()
    {
    }
}
class Extended extends Base
{
    public function __construct()
    {
    }
}

?>
--EXPECTF--
Fatal error: Cannot override final method Base::__construct() in %s on line %d
