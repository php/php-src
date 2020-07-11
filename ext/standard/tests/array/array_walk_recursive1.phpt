--TEST--
array_walk_recursive() tests
--FILE--
<?php

function foo($v1, $v2, $v3) {
    var_dump($v1);
    var_dump($v2);
    var_dump($v3);
}

$var = array(1,2, array(2,3));
var_dump(array_walk_recursive($var, "foo", "data"));

function foo2($v1, $v2, $v3) {
    throw new Exception($v3);
}

try {
    var_dump(array_walk_recursive($var,"foo2", "data"));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECT--
int(1)
int(0)
string(4) "data"
int(2)
int(1)
string(4) "data"
int(2)
int(0)
string(4) "data"
int(3)
int(1)
string(4) "data"
bool(true)
string(4) "data"
Done
