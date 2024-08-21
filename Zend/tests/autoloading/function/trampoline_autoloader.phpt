--TEST--
Test autoload_register_function(): behavior with a trampoline
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

autoload_register_function($callback1);
autoload_register_function($callback2);
autoload_register_function($callback1); // 2nd call ignored

var_dump(autoload_list_function());

var_dump(function_exists("foo", true));

echo "Unregister trampoline:\n";
var_dump(autoload_unregister_function($callback1));
var_dump(autoload_unregister_function($callback1));
var_dump(autoload_unregister_function($callback2));
var_dump(function_exists("foo", true));

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
