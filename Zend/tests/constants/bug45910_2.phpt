--TEST--
Bug #45910.2 (Cannot declare self-referencing constant)
--FILE--
<?php

class foo {
    const AAA = 'x';
    const BBB = 'a';
    const CCC = 'a';
    const DDD = self::AAA;

    private static $foo = array(
        self::BBB	=> 'a',
        self::CCC	=> 'b',
        self::DDD	=>  11
    );

    public static function test() {
        self::$foo;
    }
}

foo::test();

print 1;
?>
--EXPECT--
1
