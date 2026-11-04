--TEST--
AST can be recreated (namespace)
--EXTENSIONS--
zend_test
--FILE--
<?php

namespace Foo {
    class FooTest {
    }

    function fn_foo() {
    }

    function fn_bar() {
    }
}

namespace Bar {
    class BarTest {
    }

    function fn_bar() {
    }
}

namespace {
    use function Bar\fn_bar;
    use Foo\{function fn_foo, FooTest, function fn_bar as fn_foobar};

    new FooTest();
    fn_bar();
    fn_foobar();

    echo zend_test_compile_to_ast(file_get_contents( __FILE__ ));
}

?>
--EXPECT--
namespace Foo {
    class FooTest {
    }

    function fn_foo() {
    }

    function fn_bar() {
    }

}

namespace Bar {
    class BarTest {
    }

    function fn_bar() {
    }

}

namespace {
    use function Bar\fn_bar;
    use Foo\{function fn_foo, FooTest, function fn_bar as fn_foobar};
    new FooTest();
    fn_bar();
    fn_foobar();
    echo zend_test_compile_to_ast(file_get_contents(__FILE__));
}
