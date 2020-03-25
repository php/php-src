--TEST--
Missing interface method a object return type during inheritance
--FILE--
<?php

interface One {
    public function a() : object;
}

interface Two extends One {
    public function a();
}
--EXPECTF--
Fatal error: Declaration of Two::a() must be compatible with One::a(): object in %s on line %d
