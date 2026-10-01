--TEST--
GH-20319 005: ASSIGN_OP: var-var slot may be invalidated by __toString()
--ENV--
LEN=10
--FILE--
<?php

$v = 'b';

$$v = 'test';
var_dump($$v .= new class {
    function __toString() {
        for ($i = 0; $i < 128; $i++) {
            $GLOBALS['_' . $i] = 0;
        }
        return str_repeat('a', (int)getenv('LEN'));
    }
});

$$v = 'test';
$$v .= new class {
    function __toString() {
        for ($i = 128; $i < 512; $i++) {
            $GLOBALS['_' . $i] = 0;
        }
        return str_repeat('a', (int)getenv('LEN'));
    }
};

var_dump($$v);

?>
--EXPECT--
string(14) "testaaaaaaaaaa"
string(14) "testaaaaaaaaaa"
