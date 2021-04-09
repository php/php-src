--TEST--
Bug #79332 (php_istreams are never freed)
--EXTENSIONS--
com_dotnet
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
