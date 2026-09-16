--TEST--
Test header_register_callback
--FILE--
<?php
class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

header_register_callback($callback);
?>
--EXPECT--
Trampoline for trampoline
