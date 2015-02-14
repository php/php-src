--TEST--
Void as a class, trait or interface name - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "void");
--EXPECTF--
Fatal error: "void" cannot be used as a class name in %s on line %d
