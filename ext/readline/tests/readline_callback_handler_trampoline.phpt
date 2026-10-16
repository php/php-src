--TEST--
readline_callback_handler_install() with trampoline
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!function_exists('readline_callback_handler_install')) die("skip");
if (READLINE_LIB == "libedit") die("skip readline only");
?>
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        return 'Trampoline for ' . $name . "\n";
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

var_dump(readline_callback_handler_install('testing: ', $callback));
var_dump(readline_callback_handler_remove());
var_dump(readline_callback_handler_install('testing: ', $callback));

?>
--EXPECT--
testing: bool(true)
bool(true)
testing: bool(true)
