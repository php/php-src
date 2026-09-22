--TEST--
Use a trampoline as a shutdown function
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        var_dump($arguments);
    }
}
$o = new TrampolineTest();
$callback = [$o, 'shutdownTrampoline'];

echo "Before registering\n";

register_shutdown_function($callback, "arg1", "arg2");

echo "After registering\n";
?>
--EXPECT--
Before registering
After registering
Trampoline for shutdownTrampoline
array(2) {
  [0]=>
  string(4) "arg1"
  [1]=>
  string(4) "arg2"
}
