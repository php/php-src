--TEST--
Bug #64130 (COM obj parameters passed by reference are not updated)
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die('skip for 32bit platforms only');
try {
    $ie = new com('InternetExplorer.Application');
} catch (com_exception $ex) {
    die("skip {$ex->getMessage()}");
}
$ie->quit();
?>
--FILE--
<?php
$ie = new com('InternetExplorer.Application');
$x = 0;
$y = 0;
try {
    $ie->clientToWindow($x, $y);
} catch (com_exception $ex) {}
var_dump($x > 0, $y > 0);
$ie->quit();
?>
--EXPECT--
bool(true)
bool(true)
