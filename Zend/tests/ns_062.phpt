--TEST--
062: use \global class
--FILE--
<?php
namespace Foo;
use \StdClass;
use \StdClass as A;
echo get_class(new StdClass)."\n";
echo get_class(new A)."\n";
--EXPECT--
StdClass
StdClass
