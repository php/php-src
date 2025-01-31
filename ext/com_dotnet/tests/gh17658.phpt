--TEST--
GH-17658 (COMPersistHelper::LoadFromStream() segfaults for IPersistStream)
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
try {
    new com("PHP.Test.Document");
} catch (com_exception) {
    die("skip PHP.Test.Document not registered");
}
?>
--FILE--
<?php
$doc = new com("PHP.Test.Document");
$doc->Content = "GH-17658";
$ph = new COMPersistHelper($doc);
$stream = fopen("php://memory", "w+");
$ph->SaveToStream($stream);
$doc->Content = "";
fseek($stream, 0);
$ph->LoadFromStream($stream);
fclose($stream);
echo $doc->Content, "\n";
?>
--EXPECT--
GH-17658
