--TEST--
Nested batch use statements syntax error
--FILE--
<?php
namespace Foo\Bar\Baz {
    class A {}
    class B {}
    namespace B {
        class C {}
        class D {}
        class E {}
    }
}
namespace Fiz\Biz\Buz {
    use Foo\Bar\Baz {
        A,
        B {
            C
            D,
            E
        }
    };
}
?>
--EXPECTF--
Parse error: syntax error, unexpected '{', expecting ',' or '}' in %sns_088.php on line 14
