--TEST--
Bug #41421 (Uncaught exception from a stream wrapper segfaults)
--FILE--
<?php

class wrapper {
    function stream_open() {
        return true;
    }
    function stream_eof() {
        throw new exception();
    }
}

stream_wrapper_register("wrap", "wrapper");
$fp = fopen("wrap://...", "r");
feof($fp);

echo "Done\n";
?>
--EXPECTF--
Warning: feof(): wrapper::stream_eof is not implemented! Assuming EOF in %s on line %d

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 [internal function]: wrapper->stream_eof()
#1 %s(%d): feof(Resource id #%d)
#2 {main}
  thrown in %s on line %d
