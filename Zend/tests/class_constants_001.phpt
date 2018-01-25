--TEST--
class constants basic tests
--FILE--
<?php

class test {
	const val = "string";
	const val2 = 1;
}

var_dump(test::val);
var_dump(test::val2);

var_dump(test::val3);

echo "Done\n";
?>
--EXPECTF--
string(6) "string"
int(1)

Fatal error: Uncaught Error: Undefined class constant 'val3' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
