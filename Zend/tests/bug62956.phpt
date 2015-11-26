--TEST--
Bug #62956: "incompatible" signatures for private methods should not cause E_STRICT
--FILE--
<?php
class Base
{
	private function test()
	{}
}

class Extension extends Base
{
	private function test($arg)
	{}
}

?>
==DONE==
--EXPECT--
==DONE==
