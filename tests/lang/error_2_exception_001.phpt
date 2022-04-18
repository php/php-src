--TEST--
ZE2 errors caught as exceptions
--FILE--
<?php

class MyException extends Exception {
    function __construct(public $errno, public $errmsg) {}

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
