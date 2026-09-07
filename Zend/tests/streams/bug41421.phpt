--TEST--
Bug #41421 (Uncaught exception from a stream wrapper segfaults)
--FILE--
<?php

class wrapper {
    public $context;
    function stream_open() {
        return true;
    }
    function stream_eof() {
        throw new Exception('cannot eof');
    }
}

stream_wrapper_register("wrap", "wrapper");
$fp = fopen("wrap://...", "r");

try {
    feof($fp);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Exception: cannot eof
