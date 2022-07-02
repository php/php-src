--TEST--
Class protected constant visibility
--FILE--
<?php
class A {
    protected const protectedConst = 'protectedConst';
    static function staticConstDump() {
        var_dump(self::protectedConst);
    }
    function constDump() {
        var_dump(self::protectedConst);
    }
}

A::staticConstDump();
(new A())->constDump();
try {
    constant('A::protectedConst');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(14) "protectedConst"
string(14) "protectedConst"
Cannot access protected constant A::protectedConst
