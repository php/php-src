--TEST--
Catch Interfaces
--FILE--
<?php

interface Catchable
{
}

class MyException extends Exception implements Catchable
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
	throw new MyException($errstr, $errno, $errfile, $errline);
}

$err_msg = 'no exception';
set_error_handler('Error2Exception');

try
{
	$con = pg_connect('host=localhost dbname=xtest');
}
catch (Catchable $e)
{
	echo "Catchable\n";
}
catch (Exception $e)
{
	echo "Exception\n";
}

?>
===DONE===
--EXPECTF--
Catchable
===DONE===
