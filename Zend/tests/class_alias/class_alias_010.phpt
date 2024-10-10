--TEST--
Trying use an existing alias name in class declaration
--FILE--
<?php

interface a { }

class_alias('a', 'b');

class b { }

?>
--EXPECTF--
Warning: Cannot redeclare interface b (previously declared in %s:%d) in %s on line %d
