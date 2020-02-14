--TEST--
User-defined functions are evaluated every time in param defaults (not callable)
--FILE--
<?php

function generate_new_id() : int {
    static $id = 100;
    ++$id;
    return $id;
}

function test_id(int $id = generate_new_id()) {
    echo "id is $id\n";
}
test_id();
test_id(-1);
test_id();
?>
--EXPECTF--
Fatal error: Constant expression uses function generate_new_id() which is not in get_const_expr_functions() in %s on line 9
