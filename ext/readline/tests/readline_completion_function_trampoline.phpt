--TEST--
readline_completion_function(): Basic test
--EXTENSIONS--
readline
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        return [];
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

var_dump(readline_completion_function($callback));
var_dump(readline_completion_function('strtolower'));
var_dump(readline_completion_function($callback));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
