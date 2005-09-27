--TEST--
ZE2 __toString() in __destruct/exception
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Test
{
	function __toString()
	{
		throw new Exception("Damn!");
		return "Hello\n";
	}
	
	function __destruct()
	{
		echo $this;
	}
}

try
{
	$o = new Test;
	$o = NULL;
}
catch(Exception $e)
{
	var_dump($e->getMessage());
}

?>
====DONE====
--EXPECTF--
string(5) "Damn!"
====DONE====
--UEXPECTF--
unicode(5) "Damn!"
====DONE====
