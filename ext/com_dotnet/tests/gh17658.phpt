--TEST--
GH-17658 (COMPersistHelper::LoadFromStream() segfaults for IPersistStream)
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
try {
    new com("PHPTest.Document");
} catch (com_exception $ex) {
    die("skip PHPTest.Document not available (" . dechex($ex->getCode()) . ")");
}
?>
--FILE--
<?php
$doc = new com("PHPTest.Document");
$doc->Content = "GH-17658";
$ph = new COMPersistHelper($doc);
$stream = fopen("php://memory", "w+");
$ph->SaveToStream($stream);
$doc->Content = "";
fseek($stream, 0);
$ph->LoadFromStream($stream);
fclose($stream);
echo $doc->Content, "\n";

// verify that PHP.Test.Document does not implement IPersistStreamInit
try {
    $ph->InitNew();
} catch (com_exception $ex) {
    // supposed to fail with E_NOINTERFACE
    echo dechex($ex->getCode()), "\n";
}
?>
--EXPECT--
GH-17658
80004002
