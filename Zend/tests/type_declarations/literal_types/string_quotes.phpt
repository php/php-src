--TEST--
Literal types: single- and double-quoted string literals are equivalent
--FILE--
<?php
function a('foo' $x): string { return $x; }
function b("foo" $x): string { return $x; }

var_dump(a('foo'), a("foo"));
var_dump(b('foo'), b("foo"));

echo (new ReflectionFunction('a'))->getParameters()[0]->getType(), "\n";
echo (new ReflectionFunction('b'))->getParameters()[0]->getType(), "\n";

function c('a\'b' $x): string { return $x; }
var_dump(c("a'b"));
echo (new ReflectionFunction('c'))->getParameters()[0]->getType(), "\n";
?>
--EXPECT--
string(3) "foo"
string(3) "foo"
string(3) "foo"
string(3) "foo"
'foo'
'foo'
string(3) "a'b"
'a\'b'
