--TEST--
nullable scalar typehints
--FILE--
<?php
require 'common.inc';

function intable(int $foo) {
    var_dump($foo);
}
function intnullable(int $foo = NULL) {
    var_dump($foo);
}

intable(NULL);
intnullable(NULL);

function floatable(float $foo) {
    var_dump($foo);
}
function floatnullable(float $foo = NULL) {
    var_dump($foo);
}

floatable(NULL);
floatnullable(NULL);

function stringable(string $foo) {
    var_dump($foo);
}
function stringnullable(string $foo = NULL) {
    var_dump($foo);
}

stringable(NULL);
stringnullable(NULL);

function boolable(bool $foo) {
    var_dump($foo);
}
function boolnullable(bool $foo = NULL) {
    var_dump($foo);
}

boolable(NULL);
boolnullable(NULL);

?>
--EXPECTF--
E_RECOVERABLE_ERROR: Argument 1 passed to intable() must be of the type int, null given, called in %s on line 11 and defined in %s on line 4
int(0)
NULL
E_RECOVERABLE_ERROR: Argument 1 passed to floatable() must be of the type float, null given, called in %s on line 21 and defined in %s on line 14
float(0)
NULL
E_RECOVERABLE_ERROR: Argument 1 passed to stringable() must be of the type string, null given, called in %s on line 31 and defined in %s on line 24
string(0) ""
NULL
E_RECOVERABLE_ERROR: Argument 1 passed to boolable() must be of the type boolean, null given, called in %s on line 41 and defined in %s on line 34
bool(false)
NULL