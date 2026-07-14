--TEST--
Exception in stream wrapper + __call
--FILE--
<?php

class Loader {
    public $context;
    function stream_open() {
        return true;
    }
    function stream_read() {
        echo "stream_read\n";
        throw new Exception("Message");
    }
    function __call($m, $a) {
        echo "$m\n";
    }
}

stream_wrapper_register('abc', 'Loader');
try {
    require 'abc://';
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
stream_set_option
stream_stat
stream_read
Message
