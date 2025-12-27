--TEST--
Test basic pattern matching
--FILE--
<?php

function wrong() {
    throw new Exception();
}

class Foo {
    const FOO = 'foo';
}

var_dump(match (null) {
    is false => wrong(),
    is 0 => wrong(),
    is [] => wrong(),
    is null =>  'Literal pattern with null',
});

var_dump(match (true) {
    is false => wrong(),
    is true => 'Literal pattern with bool',
});

var_dump(match (4) {
    is 1 => wrong(),
    is 2 => wrong(),
    is 3 => wrong(),
    is 4 => 'Literal pattern with int',
    is 5 => wrong(),
    is 6 => wrong(),
});

var_dump(match ('e') {
    is 'a' => wrong(),
    is 'b' => wrong(),
    is 'c' => wrong(),
    is 'd' => wrong(),
    is 'e' => 'Literal pattern with string',
    is 'f' => wrong(),
    is 'g' => wrong(),
});

var_dump(match ('Foo') {
    is 1 => wrong(),
    is 2 => wrong(),
    is 3 => wrong(),
    is $a => 'Identifier pattern: ' . $a,
    is 5 => wrong(),
    is 6 => wrong(),
});

var_dump(match ('foo') {
    is 'bar' => wrong(),
    is Foo::FOO => 'Class constant literal',
});

?>
--EXPECT--
string(25) "Literal pattern with null"
string(25) "Literal pattern with bool"
string(24) "Literal pattern with int"
string(27) "Literal pattern with string"
string(23) "Identifier pattern: Foo"
string(22) "Class constant literal"
