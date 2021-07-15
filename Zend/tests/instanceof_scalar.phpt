--TEST--
instanceof with scalar types
--FILE--
<?php

$i = 1;
$f = 1.23;
$s = 'test';
$n = null;
$o = new stdClass;

var_dump(
    'int',
    $i instanceof int,
    $i instanceof float,
    $i instanceof object,
    $i instanceof null,
    $i instanceof string,
    
    'float',
    $f instanceof int,
    $f instanceof float,
    $f instanceof object,
    $f instanceof null,
    $f instanceof string,

    'string',
    $s instanceof int,
    $s instanceof float,
    $s instanceof object,
    $s instanceof null,
    $s instanceof string,

    'null',
    $n instanceof int,
    $n instanceof float,
    $n instanceof object,
    $n instanceof null,
    $n instanceof string,

    'object',
    $o instanceof int,
    $o instanceof float,
    $o instanceof object,
    $o instanceof null,
    $o instanceof string,
);
--EXPECT--
string(3) "int"
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
string(5) "float"
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
string(6) "string"
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
string(4) "null"
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
string(6) "object"
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)