--TEST--
GH-19844: Bail from stream_close() in zend_shutdown_executor_values()
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip Aborts with STATUS_BAD_FUNCTION_TABLE on Windows');
?>
--FILE--
<?php

class Test {
    public $context;
    private static $nested = false;

    function stream_open() {
        return true;
    }

    function stream_read() {
        return '.';
    }
    function stream_set_option() {}
    function stream_stat() {}

    function stream_eof() {
        if (!Test::$nested) {
            Test::$nested = true;
            include 'Test://';
        }
        @trigger_error('Bail', E_USER_ERROR);
    }

    function stream_close() {
        @trigger_error('Bail', E_USER_ERROR);
    }
}

stream_wrapper_register('Test', Test::class);
include 'Test://';

?>
--EXPECTF--
Fatal error: Bail in %s on line %d

Fatal error: Bail in %s on line %d

Fatal error: Bail in %s on line %d
