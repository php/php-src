--TEST--
Bug GH-10011 (Trampoline autoloader will get reregistered and cannot be unregistered)
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

spl_autoload_register($callback1);
spl_autoload_register($callback2);
spl_autoload_register($callback1); // 2nd call ignored

var_dump(spl_autoload_functions());

var_dump(class_exists("TestClass", true));

echo "Unregister trampoline:\n";
var_dump(spl_autoload_unregister($callback1));
var_dump(spl_autoload_unregister($callback1));
var_dump(spl_autoload_unregister($callback2));

var_dump(spl_autoload_functions());
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
array(0) {
}
bool(false)
