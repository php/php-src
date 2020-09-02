--TEST--
Bug #79332 (php_istreams are never freed)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
--FILE--
<?php
$ph = new COMPersistHelper(null);
try {
    $ph->LoadFromStream(fopen(__FILE__, 'r'));
} catch (com_exception $ex) {
    // use hard-coded message to avoid localization issues
    echo "A com_exception has been thrown\n";
}
?>
--EXPECT--
A com_exception has been thrown
