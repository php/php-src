--TEST--
Missing class method a object return type during inheritance
--FILE--
<?php

class One {
    public function a() : object {}
}

class Two extends One {
    public function a() {}
}
?>
--EXPECTF--
Fatal error: Declaration of Two::a() must be compatible with One::a(): object in %s on line 8
