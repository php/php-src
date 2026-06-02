--TEST--
sapi_windows_set_ctrl_handler() leak bug
--SKIPIF--
<?php
include "skipif.inc";

if (strtoupper(substr(PHP_OS, 0, 3)) !== 'WIN')
  die("skip this test is for Windows platforms only");
?>
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];

sapi_windows_set_ctrl_handler($callback);

function foo(int $event) { }

sapi_windows_set_ctrl_handler(foo(...));

echo "Done\n";

?>
--EXPECT--
Done
