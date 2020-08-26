--TEST--
Bug #64130 (COM obj parameters passed by reference are not updated)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
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
