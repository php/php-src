--TEST--
get_class_methods(): Testing with interface
--FILE--
<?php

interface A {
    function a();
    function b();
}

class B implements A {
    public function a() { }
    public function b() { }

    public function __construct() {
        var_dump(get_class_methods('A'));
        var_dump(get_class_methods('B'));
    }

    public function __destruct() { }
}

new B;

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(11) "__construct"
  [3]=>
  string(10) "__destruct"
}
