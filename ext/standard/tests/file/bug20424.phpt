--TEST--
Bug #20424 (stream_get_meta_data crashes on a normal file stream)
--FILE--
<?php
$f = fopen(dirname(__FILE__) . "/../../../../run-tests.php", "r");
$dummy = var_export(stream_get_meta_data($f), TRUE);
echo "I'm alive!\n";
?>
--EXPECT--
I'm alive!
