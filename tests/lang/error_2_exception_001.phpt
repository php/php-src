--TEST--
ZE2 errors caught as exceptions
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class MyException extends Exception {
	function __construct($_errno, $_errmsg) {
		$this->errno = $_errno;
		$this->errmsg = $_errmsg;
	}

	function getErrno() {
		return $this->errno;
	}
    
	function getErrmsg() {
		return $this->errmsg;
	}
}

function ErrorsToExceptions($errno, $errmsg) {
	throw new MyException($errno, $errmsg);
}

set_error_handler("ErrorsToExceptions");

// make sure it isn't catching exceptions that weren't
// thrown...

try {
} catch (MyException $exception) {
	echo "There was an exception: " . $exception->getErrno() . ", '" . $exception->getErrmsg() . "'\n";
}

try {
	trigger_error("I will become an exception", E_USER_ERROR);
} catch (MyException $exception) {
	echo "There was an exception: " . $exception->getErrno() . ", '" . $exception->getErrmsg() . "'\n";
}

?>
--EXPECT--
There was an exception: 256, 'I will become an exception'
