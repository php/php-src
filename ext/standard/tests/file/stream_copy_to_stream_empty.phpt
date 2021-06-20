--TEST--
stream_copy_to_stream() from empty file
--FILE--
<?php

$tmp_empty_file = __FILE__ . ".tmp";
file_put_contents($tmp_empty_file, "");

$in = fopen($tmp_empty_file, 'r');
$out = fopen('php://memory', 'w');
var_dump(stream_copy_to_stream($in, $out));

?>
--CLEAN--
<?php
unlink(__FILE__ . ".tmp");
?>
--EXPECT--
int(0)
