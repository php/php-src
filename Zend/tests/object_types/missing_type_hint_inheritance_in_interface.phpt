--TEST--
Missing interface method a object type hint during inheritance
--FILE--
<?php

interface One {
    public function a(object $obj);
}

interface Two extends One {
    public function a($obj);
}

--EXPECTF--
Fatal error: Declaration of Two::a($obj) must be compatible with One::a(object $obj) in %s on line 7