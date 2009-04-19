--TEST--
Bug #47997 (stream_copy_to_stream returns 1 on empty streams)
--FILE--
<?php

$in = fopen('data://text/plain,', 'rb+');
$out = fopen('php://memory', 'wb+');

var_dump(stream_copy_to_stream($in, $out));

?>
--EXPECT--
int(0)
