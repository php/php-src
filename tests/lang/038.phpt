--TEST--
Convert wanrings to exceptions
--FILE--
<?php

class MyException extends Exception
{
	function __construct($errstr, $errno, $errfile, $errline)
	{
		parent::__construct($errstr, $errno);
		$this->file = $errfile;
		$this->line = $errline;
	}
}

function Error2Exception($errno, $errstr, $errfile, $errline)
{
	throw new ErrorException($errstr, $errno);//, $errfile, $errline);
}

$err_msg = 'no exception';
set_error_handler('Error2Exception');

try
{
	$con = pg_connect('host=localhost dbname=xtest');
}
catch (Exception $e)
{
	$trace = $e->getTrace();
	var_dump($trace[0]['function']);
	var_dump($trace[1]['function']);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
string(15) "Error2Exception"
string(10) "pg_connect"
===DONE===
