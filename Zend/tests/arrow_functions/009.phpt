--TEST--
Arrow functions in match
--FILE--
<?php

$fn = match (true) {
    default => fn () => 1,
};
var_dump($fn());

$ret = match (true) {
    default => (fn () => 2)(),
};
var_dump($ret);

$ret = match (true) {
    default => (fn () => null)(),
};
var_dump($ret);

$fn = match (true) {
    default => fn () {
        return 3;
    },
};
var_dump($fn());

$ret = match (true) {
    default => (fn () {
        return 4;
    })(),
};
var_dump($ret);

$ret = match (true) {
    default => (fn () {})(),
};
var_dump($ret);

?>
--EXPECT--
int(1)
int(2)
NULL
int(3)
int(4)
NULL
