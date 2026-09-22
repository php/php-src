--TEST--
GH-21691 (OPcache CFG optimizer breaks reference returns with JMPZ/JMPZ_EX)
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php

function &getData() {
    return $x;
}

$data = &getData() && isset($data['key']);
var_dump($data);

?>
--EXPECT--
NULL
