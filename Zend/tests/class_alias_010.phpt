--TEST--
Trying use an existing alias name in class declaration
--FILE--
<?php

interface a { }

class_alias('a', 'b');

class b { }

?>
--EXPECTF--
Warning: interface b cannot be declared, because the name is already in use in %s on line %d
