--TEST--
085: bracketed namespace
--FILE--
<?php
namespace foo {
use \foo;
class bar {
	function __construct() {echo __METHOD__,"\n";}
}
new foo;
new bar;
}
namespace {
class foo {
	function __construct() {echo __METHOD__,"\n";}
}
use foo\bar as foo1;
new foo1;
new foo;
echo "===DONE===\n";
}
--EXPECT--
foo::__construct
foo\bar::__construct
foo\bar::__construct
foo::__construct
===DONE===