--TEST--
Strict comparison in match expression
--FILE--
<?php

function wrong() {
    throw new Exception();
}

var_dump(match (0) {
    null => wrong(),
    false => wrong(),
    0.0 => wrong(),
    [] => wrong(),
    '' => wrong(),
    0 => 'int',
});

function get_value() {
    return 0;
}

var_dump(match (get_value()) {
    null => wrong(),
    false => wrong(),
    0.0 => wrong(),
    [] => wrong(),
    '' => wrong(),
    0 => 'int',
    default => 'default',
});

?>
--EXPECT--
string(3) "int"
string(3) "int"
