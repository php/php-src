--TEST--
ZE2: set_exception_handler()
--SKIPIF--
<?php if (version_compare(zend_version(), "2.0.0-dev", "<")) print "skip Zend engine 2 required"; ?>
--FILE--
<?php
class MyException {
	function MyException($_error) {
		$this->error = $_error;	
	}
	
	function getException()
	{
		return $this->error;	
	}
}

function ThrowException()
{
	throw new MyException("'This is an exception!'");	
}


try {
} catch (MyException $exception) {
	print "There shouldn't be an exception: " . $exception->getException();
	print "\n";
}

try {
	ThrowException();	
} catch (MyException $exception) {
	print "There was an exception: " . $exception->getException();
	print "\n";
}
?>
--EXPECT--
There was an exception: 'This is an exception!'
