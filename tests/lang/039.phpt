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
    $con = fopen('/tmp/a_file_that_does_not_exist','r');
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
--EXPECT--
Catchable
