--TEST--
Bug #55372 (Literal handling in methods is inconsistent, causing memory corruption)
--FILE--
<?php

trait testTrait {
    public function testMethod() {
        if (1) {
            $letters1 = range('a', 'z', 1);
            $letters2 = range('A', 'Z', 1);
            $letters1 = 'foo';
            $letters2 = 'baarr';
            var_dump($letters1);
            var_dump($letters2);
        }
    }
}

class foo {
    use testTrait;
}

$x = new foo;
$x->testMethod();
?>
--EXPECT--
string(3) "foo"
string(5) "baarr"
