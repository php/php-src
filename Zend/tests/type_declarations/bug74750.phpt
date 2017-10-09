--TEST--
Bug #74750 (strict_types not used for builtin callbacks)
--FILE--
<?php declare(strict_types=1);

function print_str(string $foo) { var_dump($foo); }

$input = ["1", "2", 3];
array_map('print_str', $input);
--EXPECTF--	
string(1) "1"
string(1) "2"

Fatal error: Uncaught TypeError: Argument 1 passed to print_str() must be of the type string, integer given in %s
Stack trace:
#0 [internal function]: print_str(3)
#1 %s array_map('print_str', Array)
#2 {main}
  thrown in %s
