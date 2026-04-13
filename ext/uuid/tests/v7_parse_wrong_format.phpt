--TEST--
Parse UUID v4 string
--FILE--
<?php

var_dump(Uuid\UuidV7::parse("d70f042a-c5ca-4726-b259-795e47fd1b95"));

?>
--EXPECT--
NULL
