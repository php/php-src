--TEST--
Typed group use statements
--FILE--
<?php
namespace Foo\Bar {
    class A { function __construct() {echo __METHOD__,"\n";} }
    class B { function __construct() {echo __METHOD__,"\n";} }
    function fiz(){ echo __FUNCTION__,"\n"; }
    function biz(){ echo __FUNCTION__,"\n"; }
    function buz(){ echo __FUNCTION__,"\n"; }
    const FOO = 1;
    const BAR = 2;
}
namespace Fiz\Biz\Buz {

    use function Foo\Bar\{
        fiz,
        biz,
        buz as boz,
        A // <- this one must fail
    };

    use const Foo\Bar\{
        FOO as FOZ,
        BAR,
        B // <- this one must fail
    };

    use Foo\Bar\{ A, B, const BAR as BOZ };

    function buz(){ echo __FUNCTION__,"\n"; }
    const FOO = 100;

    echo "==== MIXED ====\n";
    new A();
    new B();
    var_dump(BOZ);
    echo "===== CONSTANTS ====\n";
    var_dump(FOO);
    var_dump(FOZ);
    var_dump(BAR);
    var_dump(defined('B'));
    echo "===== FUNCTIONS ====\n";
    buz();
    fiz();
    biz();
    boz();
    A();
}

--EXPECTF--
==== MIXED ====
Foo\Bar\A::__construct
Foo\Bar\B::__construct
int(2)
===== CONSTANTS ====
int(100)
int(1)
int(2)
bool(false)
===== FUNCTIONS ====
Fiz\Biz\Buz\buz
Foo\Bar\fiz
Foo\Bar\biz
Foo\Bar\buz

Fatal error: Uncaught Error: Call to undefined function Foo\Bar\A() in %sns_092.php:45
Stack trace:
#0 {main}
  thrown in %sns_092.php on line 45
