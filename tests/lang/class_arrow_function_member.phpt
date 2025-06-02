--TEST--
Arrow function as a class member
--FILE--
<?php

class a{
    public function test() {
        return 111;
    }
    public fn test2() => 222;
    private fn test3() => 333;
    static fn test4() => 444;
    protected static fn &test5() => 555;
}

$a = new a();

var_dump($a->test());
var_dump($a->test2());
var_dump($a::test4());
?>
--EXPECT--
int(111)
int(222)
int(444)