--TEST--
fopen() with relative path on a file in the script directory
--FILE--
<?php

$file = basename(__FILE__);

$fd = fopen($file, "r", true);
var_dump($fd);
fclose($fd);

?>
--EXPECTF--
resource(%d) of type (stream)
