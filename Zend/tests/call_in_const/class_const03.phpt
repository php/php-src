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
--EXPECT--
Caught Cannot call func_get_args() dynamically on line 7
Caught Cannot call func_get_args() dynamically on line 12
