--TEST--
Leak in QM_ASSIGN when unwrapping references (rc=1)
--FILE--
<?php

function &ref() {
    $str = "str";
    $str .= "str";
    return $str;
}

var_dump(true ? ref() : ref());
var_dump(ref() ?: ref());
var_dump(ref() ?? ref());

?>
--EXPECT--
string(6) "strstr"
string(6) "strstr"
string(6) "strstr"
