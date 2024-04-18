--TEST--
Trying use an existing alias name in class declaration
--FILE--
<?php

interface a { }

class_alias('a', 'b');

class b { }

?>
--EXPECTF--
Warning: Cannot declare interface b, because the name is already in use (previously declared in %s:%d) in %s on line %d
