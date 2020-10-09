--TEST--
Class private constant visibility
--FILE--
<?php
class A {
    private const privateConst = 'privateConst';
    static function staticConstDump() {
        var_dump(self::privateConst);
    }
    function constDump() {
        var_dump(self::privateConst);
    }
}

A::staticConstDump();
(new A())->constDump();
try {
    constant('A::privateConst');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(12) "privateConst"
string(12) "privateConst"
Cannot access private constant A::privateConst
