--TEST--
Bug #80933 (SplFileObject::fgets() stops at NUL byte for DROP_NEW_LINE)
--FILE--
<?php
// string with a NULL char
$line = "Lorem ipsum \0 dolor sit amet";

$temp = new SplTempFileObject();
$temp->fwrite($line);

$temp->rewind();
$read = $temp->fgets();
var_dump($line === $read); 

$temp->rewind();
$temp->setFlags(SplFileObject::DROP_NEW_LINE);
$read = $temp->fgets();
var_dump($line === $read);
?>
--EXPECT--
bool(true)
bool(true)
