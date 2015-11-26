--TEST--
081: bracketed namespace with nested unbracketed namespace
--FILE--
<?php
namespace foo {
use \foo;
class bar {
	function __construct() {echo __METHOD__,"\n";}
}
new foo;
new bar;
namespace oops;
class foo {
	function __construct() {echo __METHOD__,"\n";}
}
use foo\bar as foo1;
new foo1;
new foo;
}
?>
===DONE===
--EXPECTF--
Fatal error: Cannot mix bracketed namespace declarations with unbracketed namespace declarations in %sns_081.php on line 9
