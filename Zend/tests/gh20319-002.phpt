--TEST--
GH-20319 002: ASSIGN_OP: var-var ref lvalue may be released by __toString()
--ENV--
LEN=10
--FILE--
<?php

$b = &$c;
$v = 'b';
var_dump($$v .= new class {
    function __toString() {
        unset($GLOBALS['b'], $GLOBALS['c']);
        return str_repeat('d', (int)getenv('LEN'));
    }
});

var_dump(isset($b));

$b = &$c;
$$v .= new class {
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
