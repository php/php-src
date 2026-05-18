--TEST--
Errors: runtime accepts anything for unbounded T (mixed)
--FILE--
<?php
function f<T>(T $x): T { return $x; }

var_dump(f(42));
var_dump(f("foo"));
var_dump(f(null));
var_dump(f(new stdClass));
echo "no errors\n";
?>
--EXPECT--
int(42)
string(3) "foo"
NULL
object(stdClass)#1 (0) {
}
no errors
