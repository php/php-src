--TEST--
Non-mutating methods do not cause separation
--FILE--
<?php

struct Test {
    public function test() {
        var_dump(__METHOD__);
    }
}

$a = new Test();
$b = $a;

$a->test();
$b->test();

var_dump($a, $b);

?>
--EXPECT--
string(10) "Test::test"
string(10) "Test::test"
object(Test)#1 (0) {
}
object(Test)#1 (0) {
}
