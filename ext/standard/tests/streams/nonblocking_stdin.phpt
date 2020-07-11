--TEST--
Read from non-blocking stdio stream should not error
--FILE--
<?php

stream_set_blocking(STDIN, false);
var_dump(fread(STDIN, 1));

?>
--EXPECT--
string(0) ""
