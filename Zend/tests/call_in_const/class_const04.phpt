--TEST--
Handling call_user_func and static in class constants.
--FILE--
<?php
class MyClass {
    // There's only one constant declaration that gets shared by all of the classes.
    // So act as though the class scope is MyClass no matter where it gets called.
    const X = call_user_func('static::example');

    public static function example() {
        return static::class;
    }
    public static function main() {
        echo static::X . "\n";
    }
}
class SubClass extends MyClass {
    public static function example() {
        return "Other\n";
    }
}
// This wouldn't work without further updates to the implementation.
echo MyClass::X;
SubClass::main();
MyClass::main();

?>
--EXPECTF--
Fatal error: Constant expression uses function call_user_func() which is not in get_const_expr_functions() in %s on line 5
