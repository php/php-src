--TEST--
GH-20319 003: ASSIGN_DIM_OP: Array lvalue may be released by __toString()
--ENV--
LEN=10
--FILE--
<?php

$b = ['test'];

var_dump($b[0] .= new class {
    function __toString() {
        unset($GLOBALS['b']);
        return str_repeat('d', (int)getenv('LEN'));
    }
});

var_dump(isset($b));

unset($b);

$b = ['test'];

$b[0] .= new class {
    function __toString() {
        unset($GLOBALS['b']);
        return str_repeat('d', (int)getenv('LEN'));
    }
};

var_dump(isset($b));

?>
--EXPECT--
string(14) "testdddddddddd"
bool(false)
bool(false)
