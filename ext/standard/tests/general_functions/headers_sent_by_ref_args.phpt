--TEST--
headers_sent() by-ref argument with named arguments
--FILE--
<?php

ob_start();
$file = null;
$line = null;

$v1 = headers_sent(line: $line);
$v2 = headers_list();

echo 'headers_sent():', PHP_EOL;
var_dump($v1);
echo 'headers_list():', PHP_EOL;
var_dump($v2);
echo '$file:', PHP_EOL;
var_dump($file);
echo '$line:', PHP_EOL;
var_dump($line);

$file = null;
$line = null;
$v1 = headers_sent(filename: $file);
$v2 = headers_list();

echo 'headers_sent():', PHP_EOL;
var_dump($v1);
echo 'headers_list():', PHP_EOL;
var_dump($v2);
echo '$file:', PHP_EOL;
var_dump($file);
echo '$line:', PHP_EOL;
var_dump($line);

echo 'header():', PHP_EOL;
var_dump(header("HTTP 1.0", true, 200));

ob_end_flush();

$file = null;
$line = null;
$v1 = headers_sent(line: $line);
$v2 = headers_list();

echo 'headers_sent():', PHP_EOL;
var_dump($v1);
echo 'headers_list():', PHP_EOL;
var_dump($v2);
echo '$file:', PHP_EOL;
var_dump($file);
echo '$line:', PHP_EOL;
var_dump($line);

$file = null;
$line = null;
$v1 = headers_sent(filename: $file);
$v2 = headers_list();

echo 'headers_sent():', PHP_EOL;
var_dump($v1);
echo 'headers_list():', PHP_EOL;
var_dump($v2);
echo '$file:', PHP_EOL;
var_dump($file);
echo '$line:', PHP_EOL;
var_dump($line);

echo "Done\n";
?>
--EXPECTF--
headers_sent():
bool(false)
headers_list():
array(0) {
}
$file:
NULL
$line:
int(0)
headers_sent():
bool(false)
headers_list():
array(0) {
}
$file:
string(0) ""
$line:
NULL
header():
NULL
headers_sent():
bool(true)
headers_list():
array(0) {
}
$file:
NULL
$line:
int(36)
headers_sent():
bool(true)
headers_list():
array(0) {
}
$file:
string(%d) "%s"
$line:
NULL
Done
