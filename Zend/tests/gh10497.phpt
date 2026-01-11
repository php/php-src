--TEST--
GH-10497: Allow direct modification of object stored in a constant
--FILE--
<?php
const a = new stdClass;
a->b = 42;
var_dump(a->b);

const obj = new stdClass;
obj->foo = 'bar';
obj->baz = 123;
var_dump(obj->foo, obj->baz);

const nested = new stdClass;
nested->inner = new stdClass;
nested->inner->value = 999;
var_dump(nested->inner->value);

const readTest = new stdClass;
readTest->prop = 'test';
echo readTest->prop . "\n";

var_dump(isset(readTest->prop));
var_dump(empty(readTest->missing));

const modTest = new stdClass;
modTest->val = 1;
modTest->val = 2;
var_dump(modTest->val);
?>
--EXPECT--
int(42)
string(3) "bar"
int(123)
int(999)
test
bool(true)
bool(true)
int(2)
