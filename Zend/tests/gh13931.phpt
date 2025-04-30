--TEST--
GH-13931 (Applying zero offset to null pointer in Zend/zend_opcode.c)
--FILE--
<?php

register_shutdown_function(function() {
	var_dump(eval("return 1+3;"));
});

eval(<<<EVAL
function foo () {
    try {
        break;
    } finally {
    }
}
foo();
EVAL);

?>
--EXPECTF--
Fatal error: 'break' not in the 'loop' or 'switch' context in %s on line %d
int(4)
