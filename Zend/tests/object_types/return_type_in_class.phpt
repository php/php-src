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
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of class@anonymous::a() must be an object, integer returned in %s:13
Stack trace:
#0 %s(16): class@anonymous->a()
#1 {main}
  thrown in %s on line 13
