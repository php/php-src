--TEST--
Functions are executed in the expected order
--FILE--
<?php

function foo()     { echo __FUNCTION__, PHP_EOL; return 1; }
function bar()     { echo __FUNCTION__, PHP_EOL; return false; }
function baz($in)  { echo __FUNCTION__, PHP_EOL; return $in; }
function quux($in) { echo __FUNCTION__, PHP_EOL; return $in; }

$result = foo()
  |> (bar() ? baz(...) : quux(...))
  |> var_dump(...);

?>
--EXPECT--
foo
bar
quux
int(1)
