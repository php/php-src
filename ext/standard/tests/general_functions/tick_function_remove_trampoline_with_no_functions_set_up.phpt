--TEST--
Unregistering a trampoline as a tick function
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

unregister_tick_function($callback);

?>
OK
--EXPECT--
OK
