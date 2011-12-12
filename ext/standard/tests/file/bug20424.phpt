--TEST--
Bug #20424 (stream_get_meta_data crashes on a normal file stream)
--FILE--
<?php
$f = fopen(__FILE__, "r");
$dummy = var_export(stream_get_meta_data($f), TRUE);
echo "I'm alive!\n";
?>
--EXPECT--
I'm alive!
