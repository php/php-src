--TEST--
GH-17216 (Trampoline crash on error)
--FILE--
<?php
class TrampolineTest {
    public function __call(string $name, array $arguments) {
        var_dump($name, $arguments);
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];
$array = ["a" => "b", 1];
try {
    forward_static_call_array($callback, $array);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo "Done\n";
?>
--EXPECT--
Error: Cannot use positional argument after named argument
Done
