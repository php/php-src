--TEST--
Asymmetric visibility on static props
--FILE--
<?php

class C {
    public private(set) static int $prop;
}

?>
--EXPECTF--
Fatal error: Static property may not have asymmetric visibility in %s on line %d
