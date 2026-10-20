--TEST--
AST can be recreated (enums)
--EXTENSIONS--
zend_test
--FILE--
<?php

enum Suit
{
    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;

    public const Clubz = self::Clubs;
}

enum MyBoolean: int {
    case MyFalse = 0;
    case MyTrue = 1;

    public function toBool(): bool {
        return match($this) {
            MyBoolean::MyFalse => false,
            MyBoolean::MyTrue => true,
        };
    }
}

echo zend_test_compile_to_ast( file_get_contents( __FILE__ ) );

?>
--EXPECT--
enum Suit {
    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;
    public const Clubz = self::Clubs;
}

enum MyBoolean: int {
    case MyFalse = 0;
    case MyTrue = 1;
    public function toBool(): bool {
        return match ($this) {
            MyBoolean::MyFalse => false,
            MyBoolean::MyTrue => true,
        };
    }

}

echo zend_test_compile_to_ast(file_get_contents(__FILE__));
