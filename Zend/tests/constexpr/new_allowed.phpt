--TEST--
Places where new is allowed
--FILE--
<?php

#[SomeAttribute(new stdClass)]
class Test {
    public function __construct(
        public $prop = new stdClass,
    ) {
        var_dump($prop);
    }
}

function test($param = new stdClass) {
    static $var = new stdClass;
    var_dump($param, $var);
}

class Test3 implements Stringable {
    public function __toString(): string {
        return "hello";
    }
}

const TEST = new stdClass;
define("TEST2", new stdClass);
define("TEST3", new Test3);

new Test;
test();
var_dump(TEST);
var_dump(TEST2);
var_dump(TEST3);

?>
--EXPECT--
object(stdClass)#4 (0) {
}
object(stdClass)#3 (0) {
}
object(stdClass)#4 (0) {
}
object(stdClass)#1 (0) {
}
object(stdClass)#2 (0) {
}
string(5) "hello"
