--TEST--
stream_wrapper_unregister() & stream_wrapper_restore()
--FILE--
<?php

var_dump(stream_wrapper_unregister('file'));
var_dump(fopen("file://".__FILE__, "r"));
var_dump(stream_wrapper_restore('file'));
var_dump(fopen("file://".__FILE__, "r"));

echo "Done\n";
?>
--EXPECTF--
bool(true)

Warning: fopen(): Unable to find the wrapper "file" - did you forget to enable it when you configured PHP? in %s on line %d

Warning: fopen(): file:// wrapper is disabled in the server configuration in %s on line %d

Warning: fopen(file://%s): Failed to open stream: no suitable wrapper could be found in %s on line %d
bool(false)
bool(true)
resource(%d) of type (stream)
Done
