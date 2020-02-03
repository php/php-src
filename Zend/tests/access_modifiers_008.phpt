--TEST--
Inconsistencies when accessing protected members
--XFAIL--
Discussion: http://marc.info/?l=php-internals&m=120221184420957&w=2
--FILE--
<?php

class A
{
    protected static function f()
    {
        return 'A::f()';
    }
}
class B1 extends A
{
    protected static function f()
    {
        return 'B1::f()';
    }
}
class B2 extends A
{
    public static function test()
    {
        echo B1::f();
    }
}
B2::test();

?>
--EXPECTF--
Fatal error: Call to protected method B1::f() from context 'B2' in %s on line %d
