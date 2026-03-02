--TEST--
Test default value handling of ErrorException::__construct()
--FILE--
<?php

$e = new ErrorException();
var_dump($e->getMessage());
var_dump($e->getFile());
var_dump($e->getLine());

$e = new ErrorException("Second", 0, E_ERROR, null);
var_dump($e->getMessage());
var_dump($e->getFile());
var_dump($e->getLine());

$e = new ErrorException("Third", 0, E_ERROR, null, null);
var_dump($e->getMessage());
var_dump($e->getFile());
var_dump($e->getLine());

$e = new ErrorException("Forth", 0, E_ERROR, null, 123);
var_dump($e->getMessage());
var_dump($e->getFile());
var_dump($e->getLine());

$e = new ErrorException("Fifth", 0, E_ERROR, "abc.php");
var_dump($e->getMessage());
var_dump($e->getFile());
var_dump($e->getLine());

$e = new ErrorException("Sixth", 0, E_ERROR, "abc.php", null);
var_dump($e->getMessage());
var_dump($e->getFile());
var_dump($e->getLine());

$e = new ErrorException("Seventh", 0, E_ERROR, "abc.php", 123);
var_dump($e->getMessage());
var_dump($e->getFile());
var_dump($e->getLine());

?>
--EXPECTF--
string(0) ""
string(%d) "%sErrorException_construct.php"
int(3)
string(6) "Second"
string(%d) "%sErrorException_construct.php"
int(8)
string(5) "Third"
string(%d) "%sErrorException_construct.php"
int(13)
string(5) "Forth"
string(%d) "%sErrorException_construct.php"
int(123)
string(5) "Fifth"
string(7) "abc.php"
int(0)
string(5) "Sixth"
string(7) "abc.php"
int(0)
string(7) "Seventh"
string(7) "abc.php"
int(123)
