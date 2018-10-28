--TEST--
stristr() - New parameter: before_needle
--FILE--
<?php

$email  = 'AbcCdEfGh';
var_dump(stristr($email, 'c'));
var_dump(stristr($email, 'c', 1));

$email  = 'AbCdeEfGh';
var_dump(stristr($email, 'E'));
var_dump(stristr($email, 'E', 1));

$email  = 'wazAbCdeEfGh';
var_dump(stristr($email, 97));
var_dump(stristr($email, 97, 1));

?>
--EXPECTF--
string(7) "cCdEfGh"
string(2) "Ab"
string(5) "eEfGh"
string(4) "AbCd"

Deprecated: stristr(): Non-string needles will be interpreted as strings in the future. Use an explicit chr() call to preserve the current behavior in %s on line %d
string(11) "azAbCdeEfGh"

Deprecated: stristr(): Non-string needles will be interpreted as strings in the future. Use an explicit chr() call to preserve the current behavior in %s on line %d
string(1) "w"
