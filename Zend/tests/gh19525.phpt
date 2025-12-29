--TEST--
GH-19525: Stream wrapper stream_close() not called on exception unwind
--FILE--
<?php

class Loader {
    public $context;

    public function stream_open()  {
        return true;
    }
    function stream_read() {}
    function stream_eof() {}
    function stream_flush() {}
    function stream_close() {
        echo __METHOD__, "\n";
    }
}

function foo() {
    stream_wrapper_register('Loader', 'Loader');
    $fp = fopen('Loader://qqq.php', 'r');
    throw new Exception();
}
foo();

?>
--EXPECTF--
Loader::stream_close

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}
  thrown in %s on line %d
