--TEST--
nullable scalar typehints
--FILE--
<?php
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
int(0)
NULL
float(0)
NULL
string(0) ""
NULL
bool(false)
NULL