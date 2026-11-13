--TEST--
Test str ZPP specifier (weak_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php
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

Deprecated: zend_str(): Passing null to parameter #1 ($param) of type string is deprecated in %s on line %d
string(0) ""
NULL

Deprecated: zend_str_slow_zpp(): Passing null to parameter #1 ($param) of type string is deprecated in %s on line %d
string(0) ""
NULL

Deprecated: zend_str(): Passing null to parameter #1 ($param) of type string is deprecated in %s on line %d
string(0) ""
NULL

Deprecated: zend_str_slow_zpp(): Passing null to parameter #1 ($param) of type string is deprecated in %s on line %d
string(0) ""
NULL
Using false:
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
Using true:
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
Using 42:
string(2) "42"
string(2) "42"
string(2) "42"
string(2) "42"
string(2) "42"
string(2) "42"
string(2) "42"
string(2) "42"
Using 73.5:
string(4) "73.5"
string(4) "73.5"
string(4) "73.5"
string(4) "73.5"
string(4) "73.5"
string(4) "73.5"
string(4) "73.5"
string(4) "73.5"
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
string(7) "S class"
string(7) "S class"
string(7) "S class"
string(7) "S class"
string(7) "S class"
string(7) "S class"
string(7) "S class"
string(7) "S class"
Using STDOUT:
TypeError: zend_str(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, resource given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, resource given
TypeError: zend_str(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_str_or_null(): Argument #1 ($param) must be of type ?string, resource given
TypeError: zend_str_slow_zpp(): Argument #1 ($param) must be of type string, resource given
TypeError: zend_str_or_null_slow_zpp(): Argument #1 ($param) must be of type ?string, resource given
