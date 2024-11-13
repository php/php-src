--TEST--
Property name lookup error
--EXTENSIONS--
com_dotnet
--FILE--
<?php
$dict = new COM("Scripting.Dictionary");
try {
    $dict->unknownProperty;
} catch (com_exception $ex) {
    echo $ex->getMessage(), "\n";
}
?>
--EXPECTF--
Unable to lookup `unknownProperty': %s
