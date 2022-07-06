--TEST--
Bug #69125 (Array numeric string as key)
--INI--
opcache.enable=1
opcache.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

const SZERO = '0';
const SONE = '1';

$array[SZERO] = "okey";
$array[1] = "okey";

var_dump($array[SZERO]);
var_dump($array[SONE]);
?>
--EXPECT--
string(4) "okey"
string(4) "okey"
