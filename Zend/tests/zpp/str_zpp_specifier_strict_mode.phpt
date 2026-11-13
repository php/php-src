--TEST--
Test str ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php
declare(strict_types=1);
$types = require 'types.inc';

foreach ($types as $name => $type) {
    echo "Using $name:\n";

    try {
        var_dump(zend_str($type));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        var_dump(zend_str_or_null($type));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        var_dump(zend_str_slow_zpp($type));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        var_dump(zend_str_or_null_slow_zpp($type));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    $ref =& $type;

    try {
        var_dump(zend_str($ref));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        var_dump(zend_str_or_null($ref));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        var_dump(zend_str_slow_zpp($ref));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        var_dump(zend_str_or_null_slow_zpp($ref));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
Using null:
TypeError: zend_str(): Argument #1 ($param) must be of type string, null given
NULL
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, null given
NULL
TypeError: zend_str(): Argument #1 ($param) must be of type string, null given
NULL
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, null given
NULL
Using false:
TypeError: zend_str(): Argument #1 ($param) must be of type string, false given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, false given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, false given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, false given
TypeError: zend_str(): Argument #1 ($param) must be of type string, false given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, false given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, false given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, false given
Using true:
TypeError: zend_str(): Argument #1 ($param) must be of type string, true given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, true given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, true given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, true given
TypeError: zend_str(): Argument #1 ($param) must be of type string, true given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, true given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, true given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, true given
Using 42:
TypeError: zend_str(): Argument #1 ($param) must be of type string, int given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, int given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, int given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, int given
TypeError: zend_str(): Argument #1 ($param) must be of type string, int given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, int given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, int given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, int given
Using 73.5:
TypeError: zend_str(): Argument #1 ($param) must be of type string, float given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, float given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, float given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, float given
TypeError: zend_str(): Argument #1 ($param) must be of type string, float given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, float given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, float given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, float given
Using 'string':
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
string(6) "string"
Using '15':
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
Using '56.7':
string(4) "56.7"
string(4) "56.7"
string(4) "56.7"
string(4) "56.7"
string(4) "56.7"
string(4) "56.7"
string(4) "56.7"
string(4) "56.7"
Using 'stdClass':
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
string(8) "stdClass"
Using anon class name:
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
string(%d) "class@anonymous%s"
Using []:
TypeError: zend_str(): Argument #1 ($param) must be of type string, array given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, array given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, array given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, array given
TypeError: zend_str(): Argument #1 ($param) must be of type string, array given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, array given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, array given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, array given
Using new stdClass():
TypeError: zend_str(): Argument #1 ($param) must be of type string, stdClass given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, stdClass given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, stdClass given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, stdClass given
TypeError: zend_str(): Argument #1 ($param) must be of type string, stdClass given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, stdClass given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, stdClass given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, stdClass given
Using new S():
TypeError: zend_str(): Argument #1 ($param) must be of type string, S given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, S given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, S given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, S given
TypeError: zend_str(): Argument #1 ($param) must be of type string, S given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, S given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, S given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, S given
Using STDOUT:
TypeError: zend_str(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, resource given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, resource given
TypeError: zend_str(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, resource given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, resource given
