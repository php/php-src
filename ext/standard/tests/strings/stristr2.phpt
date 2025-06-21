--TEST--
stristr() - New parameter: before_needle
--FILE--
<?php

$email  = 'AbcCdEfGh';
var_dump(stristr($email, 'c'));
var_dump(stristr($email, 'c', true));

$email  = 'AbCdeEfGh';
var_dump(stristr($email, 'E'));
var_dump(stristr($email, 'E', true));

$email  = 'wazAbCdeEfGh';
var_dump(stristr($email, 97));
var_dump(stristr($email, 97, true));

?>
--EXPECT--
string(7) "cCdEfGh"
string(2) "Ab"
string(5) "eEfGh"
string(4) "AbCd"
bool(false)
bool(false)
