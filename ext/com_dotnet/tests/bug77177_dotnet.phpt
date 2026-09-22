--TEST--
Bug #77177 (Serializing or unserializing COM objects crashes, dotnet class)
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
if (!class_exists("dotnet")) die("skip mscoree not available");
?>
--FILE--
<?php
$dotnet = new DOTNET("mscorlib", "System.Collections.Stack");
try {
    serialize($dotnet);
} catch (Exception $ex) {
    echo "Exception: {$ex->getMessage()}\n";
}

try {
    unserialize('C:6:"dotnet":0:{}');
} catch (Exception $ex) {
    echo "Exception: {$ex->getMessage()}\n";
}

try {
    unserialize('O:6:"dotnet":0:{}');
} catch (Exception $ex) {
    echo "Exception: {$ex->getMessage()}\n";
}
?>
--EXPECT--
Exception: Serialization of 'dotnet' is not allowed
Exception: Unserialization of 'dotnet' is not allowed
Exception: Unserialization of 'dotnet' is not allowed
