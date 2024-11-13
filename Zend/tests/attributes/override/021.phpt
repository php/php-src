--TEST--
#[\Override]: __construct
--FILE--
<?php

class P {
    public function __construct() {}
}

class C extends P {
    #[\Override]
    public function __construct() {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::__construct() has #[\Override] attribute, but no matching parent method exists in %s on line %d
