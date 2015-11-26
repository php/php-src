--TEST--
testing the behavior of string offsets
--INI--
error_reporting=E_ALL | E_DEPRECATED
--FILE--
<?php
$string = "foobar";
var_dump($string[0]);
var_dump($string[1]);
var_dump(isset($string[0]));
var_dump(isset($string[0][0]));
var_dump($string["foo"]);
var_dump(isset($string["foo"]["bar"]));
var_dump($string{0});
var_dump($string{1});
var_dump(isset($string{0}));
var_dump(isset($string{0}{0}));
var_dump($string{"foo"});
var_dump(isset($string{"foo"}{"bar"}));
?>
--EXPECTF--
string(1) "f"
string(1) "o"
bool(true)
bool(true)

Warning: Illegal string offset 'foo' in %s line %d
string(1) "f"
bool(false)
string(1) "f"
string(1) "o"
bool(true)
bool(true)

Warning: Illegal string offset 'foo' in %s line %d
string(1) "f"
bool(false)

