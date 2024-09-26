--TEST--
ZE2 A derived class with an abstract method must be abstract
--FILE--
<?php

class base {
}

class derived extends base {
    abstract function show();
}

?>
===DONE===
--EXPECTF--
Fatal error: Class derived contains abstract method derived::show and must therefore be declared abstract in %sabstract_derived.php on line %d
