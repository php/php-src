--TEST--
ZE2 __toString() in __destruct
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Test
{
	function __toString()
	{
		return "Hello\n";
	}
	
	function __destruct()
	{
		echo $this;
	}
}

$o = new Test;
$o = NULL;

$o = new Test;

?>
====DONE====
--EXPECTF--
Hello
====DONE====
Hello
