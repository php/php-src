--TEST--
GH-20319 001: ASSIGN_OP: Ref lvalue may be released by __toString()
--ENV--
LEN=10
--FILE--
<?php

$b = &$c;
var_dump($b .= new class {
    function __toString() {
        unset($GLOBALS['b'], $GLOBALS['c']);
        return str_repeat('d', (int)getenv('LEN'));
    }
});

var_dump(isset($b));

$b = &$c;
$b .= new class {
    function __toString() {
        unset($GLOBALS['b'], $GLOBALS['c']);
        return str_repeat('d', (int)getenv('LEN'));
    }
};

var_dump(isset($b));

?>
--EXPECT--
string(10) "dddddddddd"
bool(false)
bool(false)
