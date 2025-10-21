--TEST--
SplFileObject::valid() with non-seekable streams should not hang
--FILE--
<?php
$file = new SplFileObject("php://stdin", "r");
var_dump($file->valid());
?>
--STDIN--

--EXPECT--
bool(true)
