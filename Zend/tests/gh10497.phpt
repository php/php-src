--TEST--
GH-10497: Allow direct modification of object properties on constants
--FILE--
<?php

const OBJ = new stdClass;
OBJ->prop = 123;
var_dump(OBJ->prop);

OBJ->foo = 'bar';
OBJ->baz = 456;
var_dump(OBJ->foo, OBJ->baz);

OBJ->prop = 'overwritten';
var_dump(OBJ->prop);

OBJ->inner = new stdClass;
OBJ->inner->value = 999;
var_dump(OBJ->inner->value);

OBJ->counter = 0;
OBJ->counter++;
OBJ->counter++;
OBJ->counter--;
var_dump(OBJ->counter);

OBJ->str = 'hello';
OBJ->str .= ' world';
var_dump(OBJ->str);

OBJ->temp = 'remove me';
var_dump(isset(OBJ->temp));
unset(OBJ->temp);
var_dump(isset(OBJ->temp));

var_dump(isset(OBJ->foo));
var_dump(empty(OBJ->foo));
var_dump(isset(OBJ->nonexistent));
var_dump(empty(OBJ->nonexistent));

function incr(&$v) { $v++; }
OBJ->reftest = 10;
incr(OBJ->reftest);
var_dump(OBJ->reftest);

?>
--EXPECT--
int(123)
string(3) "bar"
int(456)
string(11) "overwritten"
int(999)
int(1)
string(11) "hello world"
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
int(11)
