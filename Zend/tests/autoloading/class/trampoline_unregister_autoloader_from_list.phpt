--TEST--
Unregister all class autoloaders by traversing the registered list: behavior with a trampoline
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

var_dump(autoload_list_class());

foreach (autoload_list_class() as $func) {
    autoload_unregister_class($func);
}

var_dump(autoload_list_class());
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
array(0) {
}
