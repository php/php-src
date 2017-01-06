--TEST--
Missing interface method a object return type during inheritance
--FILE--
<?php

interface One {
    public function a() : object {}
}

interface Two extends One {
    public function a() {}
}

--EXPECTF--
Fatal error: Interface function One::a() cannot contain body in %s on line 4