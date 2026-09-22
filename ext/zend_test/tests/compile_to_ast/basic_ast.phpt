--TEST--
AST can be recreated (basic example)
--EXTENSIONS--
zend_test
--FILE--
<?php

/**
 * My documentation comment
 */
#[MyAttrib]
function doSomething( int $intParam, string $stringParam ): object {
    return (object)[ $intParam, $stringParam ];
}

// Inline comment

const MY_CONSTANT = true;

class Example {
    private int $value;

    public function __construct( int $v ) {
        $this->value = $v;
    }

    public function getValue(): int {
        return $this->value;
    }
}

echo zend_test_compile_to_ast( file_get_contents( __FILE__ ) );

?>
--EXPECT--
#[MyAttrib]
function doSomething(int $intParam, string $stringParam): object {
    return (object)[$intParam, $stringParam];
}

const MY_CONSTANT = true;
class Example {
    private int $value;
    public function __construct(int $v) {
        $this->value = $v;
    }

    public function getValue(): int {
        return $this->value;
    }

}

echo zend_test_compile_to_ast(file_get_contents(__FILE__));
