--TEST--
Bug #20424: stream_get_meta_data craches on a normal file stream
--POST--
--GET--
--FILE--
<?php
$f = fopen("run-tests.php", "r");
$dummy = var_export(stream_get_meta_data($f), TRUE);
echo "I'm alive!\n";
?>
--EXPECT--
I'm alive!
