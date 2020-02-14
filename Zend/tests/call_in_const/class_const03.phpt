--TEST--
Cannot call functions accessing the variable scope in class constants
--FILE--
<?php
class Example {
    const X = func_get_args();

    public static function main($value) {
        try {
            var_export(self::X);
        } catch (Error $e) {
            printf("Caught %s on line %d\n", $e->getMessage(), $e->getLine());
        }
        try {
            var_export(self::X);
        } catch (Error $e) {
            printf("Caught %s on line %d\n", $e->getMessage(), $e->getLine());
        }
    }
}
Example::main('test');
?>
--EXPECTF--
Fatal error: Constant expression uses function func_get_args() which is not in get_const_expr_functions() in %s on line 3
