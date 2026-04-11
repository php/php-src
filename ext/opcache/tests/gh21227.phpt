--TEST--
GH-21227: Borked SCCP of array containing partial object
--CREDITS--
Daniel Chong (chongwick)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function test() {
    $obj->a = 3;
    $objs = [];
    $obj = new stdClass;
    $objs[] = $obj;
}

?>
===DONE===
--EXPECT--
===DONE===
