--TEST--
Adding interface method a object return type during inheritance is allowed
--FILE--
<?php

interface One {
    public function a();
}

interface Two extends One {
    public function a() : object;
}

$three = new class implements Two {
    public function a() : object {
        return 12345;
    }
};
$three->a();
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of Two@anonymous::a() must be of type object, int returned in %s:%d
Stack trace:
#0 %s(%d): Two@anonymous->a()
#1 {main}
  thrown in %s on line 13
