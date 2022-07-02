--TEST--
Bug #77177 (Serializing or unserializing COM objects crashes)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
$com = new COM("WScript.Shell");
$dotnet = new DOTNET("mscorlib", "System.Collections.Stack");
$variant = new VARIANT;
foreach ([$com, $dotnet, $variant] as $object) {
    try {
        serialize($object);
    } catch (Exception $ex) {
        echo "Exception: {$ex->getMessage()}\n";
    }
}

$strings = ['C:3:"com":0:{}', 'C:6:"dotnet":0:{}', 'C:7:"variant":0:{}'];
foreach ($strings as $string) {
    try {
        unserialize($string);
    } catch (Exception $ex) {
        echo "Exception: {$ex->getMessage()}\n";
    }
}

$strings = ['O:3:"com":0:{}', 'O:6:"dotnet":0:{}', 'O:7:"variant":0:{}'];
foreach ($strings as $string) {
    try {
        unserialize($string);
    } catch (Exception $ex) {
        echo "Exception: {$ex->getMessage()}\n";
    }
}
?>
--EXPECTF--
Exception: Serialization of 'com' is not allowed
Exception: Serialization of 'dotnet' is not allowed
Exception: Serialization of 'variant' is not allowed
Exception: Unserialization of 'com' is not allowed
Exception: Unserialization of 'dotnet' is not allowed
Exception: Unserialization of 'variant' is not allowed
Exception: Unserialization of 'com' is not allowed
Exception: Unserialization of 'dotnet' is not allowed
Exception: Unserialization of 'variant' is not allowed
