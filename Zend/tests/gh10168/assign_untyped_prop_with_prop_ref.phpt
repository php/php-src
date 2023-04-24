--TEST--
GH-10168: Assign prop with prop ref
--FILE--
<?php

class Box {
    public $value;
}

class Test {
    static ?Test $test;

    function __destruct() {
        global $box;
        $box->value = null;
    }
}

function test($box) {
    var_dump($box->value = new Test);
}

$box = new Box();
$box->value = new Test;
Test::$test = &$box->value;
test($box);
// Second call tests the cache slot path
test($box);

?>
--EXPECT--
object(Test)#3 (0) {
}
object(Test)#3 (0) {
}
