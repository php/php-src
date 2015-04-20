--TEST--
061: use in global scope
--FILE--
<?php
use \A as B;

class A {}
echo get_class(new B)."\n";
--EXPECT--
A
