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

echo "Send headers.\n";
header_register_callback($callback);
?>
--EXPECT--
Send headers.
