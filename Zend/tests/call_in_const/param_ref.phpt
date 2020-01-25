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
int(1)

Warning: Parameter 3 to preg_match() expected to be a reference, value given in %s on line 12
int(1)
array(0) {
}
