--TEST--
AST can be recreated (complicated class example)
--EXTENSIONS--
zend_test
--FILE--
<?php

interface MyInterface1 {}
interface MyInterface2 extends MyInterface1 {}

abstract class MyBaseClass implements MyInterface2 {
    final public function get5(): int {
        return 5;
    }

    abstract public function setValue( string $value ): void;
}

class MySubClass extends MyBaseClass {
    public private(set) string $alwaysLowerCase = "STARTS UPPER" {
        get => $this->alwaysLowerCase;
        set => strtolower( $value );
    }

    public readonly bool $negated;
    
    public const CONST_PUBLIC = 1;
    protected const CONST_PROTECTED = "abc";
    private const CONST_PRIVATE = [ 1, 2 ];

    public static ?object $cache = null;

    public function __construct( public bool $boolVal ) {
        $this->negated = !$boolVal;
    }

    public function setValue( string $value ): void {
        $this->alwaysLowerCase = $value;
    }

    public static function getPrivateConst(): array {
        return self::CONST_PRIVATE;
    }
}

echo zend_test_compile_to_ast( file_get_contents( __FILE__ ) );

?>
--EXPECT--
interface MyInterface1 {
}

interface MyInterface2 implements MyInterface1 {
}

abstract class MyBaseClass implements MyInterface2 {
    public final function get5(): int {
        return 5;
    }

    public abstract function setValue(string $value): void;

}

class MySubClass extends MyBaseClass {
    public private(set) string $alwaysLowerCase = 'STARTS UPPER' {
        get => $this->alwaysLowerCase;
        set => strtolower($value);
    }
    public readonly bool $negated;
    public const CONST_PUBLIC = 1;
    protected const CONST_PROTECTED = 'abc';
    private const CONST_PRIVATE = [1, 2];
    public static ?object $cache = null;
    public function __construct(public bool $boolVal) {
        $this->negated = !$boolVal;
    }

    public function setValue(string $value): void {
        $this->alwaysLowerCase = $value;
    }

    public static function getPrivateConst(): array {
        return self::CONST_PRIVATE;
    }

}

echo zend_test_compile_to_ast(file_get_contents(__FILE__));
