--TEST--
Test autoload_unregister_class(): behavior with a trampoline
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
    }
}
$o = new TrampolineTest();
$callback1 = [$o, 'trampoline1'];
$callback2 = [$o, 'trampoline2'];

autoload_register_class($callback1);
autoload_register_class($callback2);
autoload_register_class($callback1); // 2nd call ignored

var_dump(autoload_list_class());

var_dump(class_exists("TestClass", true));

echo "Unregister trampoline:\n";
var_dump(autoload_unregister_class($callback1));
var_dump(autoload_unregister_class($callback1));
var_dump(autoload_unregister_class($callback2));
var_dump(class_exists("TestClass", true));

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    object(TrampolineTest)#1 (0) {
    }
    [1]=>
    string(11) "trampoline1"
  }
  [1]=>
  array(2) {
    [0]=>
    object(TrampolineTest)#1 (0) {
    }
    [1]=>
    string(11) "trampoline2"
  }
}
Trampoline for trampoline1
Trampoline for trampoline2
bool(false)
Unregister trampoline:
bool(true)
bool(false)
bool(true)
bool(false)
