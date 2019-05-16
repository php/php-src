--TEST--
If the LHS of ref-assign ERRORs, that takes precedence over the "only variables" notice
--FILE--
<?php

function val() {
    return 42;
}

$str = "foo";
$var = 24;
var_dump($str->foo =& $var);
var_dump($str);
var_dump($str->foo =& val());
var_dump($str);

?>
--EXPECTF--
Warning: Attempt to modify property 'foo' of non-object in %s on line %d
NULL
string(3) "foo"

Warning: Attempt to modify property 'foo' of non-object in %s on line %d
NULL
string(3) "foo"
