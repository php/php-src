--TEST--
Exception with by-ref message
--FILE--
<?php

class MyException extends Exception
{
    public function __construct(&$msg) {
        $this->message =& $msg;
    }
}

$msg = 'Message';
throw new MyException($msg);

?>
--EXPECTF--
MyException: Message in %s on line %d
Stack trace:
#0 {main}
