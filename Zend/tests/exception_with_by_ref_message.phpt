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
Fatal error: Uncaught exception 'MyException' with message 'Message' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
