--TEST--
Adding a class method object return type
--FILE--
<?php

interface One {
    public function a() : object;
}

class Two implements One {
    public function a() : object {}
}

$three = new class extends Two {
    public function a() : object {
        return 12345;
    }
};
$three->a();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Two@anonymous::a(): Return value must be of type object, int returned in %s:%d
Stack trace:
#0 %s(%d): Two@anonymous->a()
#1 {main}
  thrown in %s on line 13
