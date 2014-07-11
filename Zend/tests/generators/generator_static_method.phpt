--TEST--
A static method can be a generator
--FILE--
<?php

class Test {
    public static function gen() {
        var_dump(get_class());
        var_dump(get_called_class());
        yield 1;
        yield 2;
        yield 3;
    }
}

class ExtendedTest extends Test {
}

foreach (ExtendedTest::gen() as $i) {
    var_dump($i);
}

?>
--EXPECT--
string(4) "Test"
string(12) "ExtendedTest"
int(1)
int(2)
int(3)
