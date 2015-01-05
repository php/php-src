--TEST--
Bug #48228 (crash when exception is thrown while passing function arguments)
--FILE--
<?php

function do_throw() {
	throw new Exception();
}

class aa 
{
	function check()
	{
	}

	function dosome()
	{
		$this->check(do_throw());
	}
}
$l_aa=new aa();

$l_aa->dosome();
?>
--EXPECTF--

Fatal error: Uncaught exception 'Exception' in %s
Stack trace:
#0 %s(%d): do_throw()
#1 %s(%d): aa->dosome()
#2 {main}
  thrown in %s
