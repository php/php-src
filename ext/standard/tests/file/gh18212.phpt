--TEST--
GH-18212: fseek with SEEK_CUR and negative offset leads to negative file stream position.
--FILE--
<?php
$fp = fopen('php://input', 'r+');
var_dump(fseek($fp, -1, SEEK_SET));
var_dump(fseek($fp, -32, SEEK_CUR));
fclose($fp);
?>
--EXPECT--
int(-1)
int(-1)

