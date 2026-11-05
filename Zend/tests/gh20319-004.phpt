--TEST--
GH-20319 004: ASSIGN_DIM_OP: Array dim lvalue may be released by __toString()
--FILE--
<?php

$t = 'test';
$b = [&$t];

var_dump($b[0] .= new class {
    function __toString() {
        global $b;
        unset($b[0]);
        return str_repeat('d', (int)getenv('LEN'));
    }
});

var_dump($b);

unset($b);
unset($t);

$t = 'test';
$b = [&$t];

$b[0] .= new class {
    function __toString() {
        global $b;
        unset($b[0]);
        return str_repeat('d', (int)getenv('LEN'));
    }
};

var_dump($b);

?>
--EXPECT--
string(4) "test"
array(0) {
}
array(0) {
}
