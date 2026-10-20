--TEST--
array_find(): with trampoline
--FILE--
<?php
$array = [
    "a" => 1,
    "b" => 2,
    "c" => 3,
    "d" => 4,
    "e" => 5,
];

class TrampolineTest {
    public function __call(string $name, array $arguments): string {
        echo 'Trampoline for ', $name, PHP_EOL;
  		return $arguments[0] % 2 === 0;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

var_dump(array_find($array, $callback));

?>
--EXPECT--
Trampoline for trampoline
Trampoline for trampoline
int(2)
