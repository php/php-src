--TEST--
ZE2 cannot override final __construct
--FILE--
<?php

class Base
{
    public final function __construct()
    {
    }
}

class Works extends Base
{
}

class Extended extends Base
{
    public function __construct()
    {
    }
}

?>
--EXPECTF--
Fatal error: Method Extended::__construct() cannot override final method Base::__construct() in %s on line %d
