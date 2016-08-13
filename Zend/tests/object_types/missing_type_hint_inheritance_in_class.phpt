--TEST--
Missing class method a object type hint during inheritance
--FILE--
<?php

class One {
    public function a(object $obj) {}
}

class Two extends One {
    public function a($obj) {}
}

--EXPECTF--
Warning: Declaration of Two::a($obj) should be compatible with One::a(object $obj) in %s on line 9