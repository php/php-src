--TEST--
Testing for regression with encapsed variables in class declaration context
--FILE--
<?php

class A { function foo() { "{${$a}}"; } function list() {} }

echo "Done", PHP_EOL;

?>
--EXPECT--
Done
