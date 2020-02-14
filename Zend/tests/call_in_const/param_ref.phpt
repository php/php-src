--TEST--
Warn when calling function expecting a reference as an argument
--INI--
error_reporting=E_ALL
--FILE--
<?php
class Example {
    const VALUES = [];
    const IS_MATCH = preg_match('/test/', 'testing');
    const IS_MATCH_V2 = preg_match('/test/', 'testing', self::VALUES);

    public static function main() {
        echo "X is " . self::X . "\n";
    }
}
var_dump(Example::IS_MATCH);
var_dump(Example::IS_MATCH_V2);
var_dump(Example::VALUES);
--EXPECTF--
Fatal error: Constant expression uses function preg_match() which is not in get_const_expr_functions() in %s on line 4
