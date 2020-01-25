--TEST--
User-defined functions are evaluated every time in param defaults
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
--EXPECT--
id is 101
id is -1
id is 102
