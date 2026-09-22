--TEST--
Use a trampoline as a tick function
--FILE--
<?php
declare(ticks=1);

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        var_dump($arguments);
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

register_tick_function($callback, "arg1", "arg2");

echo "Tick function should run\n";

unregister_tick_function($callback);

echo "Tick function should be removed and not run\n";
$o->notTickTrampoline("not in tick");

?>
--EXPECT--
Trampoline for trampoline
array(2) {
  [0]=>
  string(4) "arg1"
  [1]=>
  string(4) "arg2"
}
Tick function should run
Trampoline for trampoline
array(2) {
  [0]=>
  string(4) "arg1"
  [1]=>
  string(4) "arg2"
}
Tick function should be removed and not run
Trampoline for notTickTrampoline
array(1) {
  [0]=>
  string(11) "not in tick"
}
