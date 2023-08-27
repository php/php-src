--TEST--
Bug #52335 (fseek() on memory stream behavior different then file)
--FILE--
<?php

echo "Read mode\n";
$fpr = fopen("php://memory", "r");
var_dump(fseek($fpr, 20));
var_dump(feof($fpr));
var_dump(ftell($fpr));
var_dump(feof($fpr));
var_dump(fread($fpr, 2));
var_dump(feof($fpr));
var_dump(fseek($fpr, 24));
var_dump(feof($fpr));
var_dump(ftell($fpr));
fclose($fpr);

echo "Read write mode\n";
$fprw = fopen("php://memory", "r+");
var_dump(fwrite($fprw, "data"));
var_dump(fseek($fprw, 20, SEEK_END));
var_dump(feof($fprw));
var_dump(ftell($fprw));
var_dump(feof($fprw));
var_dump(fread($fprw, 2));
var_dump(feof($fprw));
var_dump(fseek($fprw, 20));
var_dump(fwrite($fprw, " and more data"));
var_dump(feof($fprw));
var_dump(ftell($fprw));
var_dump(fread($fprw, 10));
var_dump(fseek($fprw, 16, SEEK_CUR));
var_dump(ftell($fprw));
var_dump(fseek($fprw, 0));
var_dump(bin2hex(stream_get_contents($fprw)));
fclose($fprw);

?>
--EXPECT--
Read mode
int(0)
bool(false)
int(20)
bool(false)
string(0) ""
bool(true)
int(0)
bool(false)
int(24)
Read write mode
int(4)
int(0)
bool(false)
int(24)
bool(false)
string(0) ""
bool(true)
int(0)
int(14)
bool(false)
int(34)
string(0) ""
int(0)
int(50)
int(0)
string(68) "646174610000000000000000000000000000000020616e64206d6f72652064617461"
