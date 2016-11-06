--TEST--
Adding a interface method object return type
--FILE--
<?php

interface One {
    public function a() : object;
}

$two = new class implements One {
    public function a() : object {
        return 12345;
    }
};
$two->a();
--EXPECTF--

Fatal error: Uncaught TypeError: Return value of class@anonymous::a() must be an object, integer returned in %s:9
Stack trace:
#0 %s(12): class@anonymous->a()
#1 {main}
  thrown in %s on line 9