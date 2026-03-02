--TEST--
Testing creation of alias using an existing interface name
--FILE--
<?php

class foo { }

interface test { }


class_alias('foo', 'test');

?>
--EXPECTF--
Warning: Cannot redeclare class test (previously declared in %s:%d) in %s on line %d
