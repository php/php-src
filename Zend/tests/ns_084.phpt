--TEST--
084: unbracketed namespace with nested bracketed namespace
--FILE--
<?php
namespace foo;
use \foo;
class bar {
	function __construct() {echo __METHOD__,"\n";}
}
new foo;
new bar;
namespace oops {
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
Fatal error: Cannot mix bracketed namespace declarations with unbracketed namespace declarations in %sns_084.php on line 9
