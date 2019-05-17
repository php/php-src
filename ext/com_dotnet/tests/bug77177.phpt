--TEST--
Bug #77177 (Serializing or unserializing COM objects crashes)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
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
    var_dump(unserialize($string));
}
?>
===DONE===
--EXPECTF--
Exception: Serialization of 'com' is not allowed
Exception: Serialization of 'dotnet' is not allowed
Exception: Serialization of 'variant' is not allowed
Exception: Unserialization of 'com' is not allowed
Exception: Unserialization of 'dotnet' is not allowed
Exception: Unserialization of 'variant' is not allowed

Warning: Erroneous data format for unserializing 'com' in %s on line %d

Notice: unserialize(): Error at offset 13 of 14 bytes in %s on line %d
bool(false)

Warning: Erroneous data format for unserializing 'dotnet' in %s on line %d

Notice: unserialize(): Error at offset 16 of 17 bytes in %s on line %d
bool(false)

Warning: Erroneous data format for unserializing 'variant' in %s on line %d

Notice: unserialize(): Error at offset 17 of 18 bytes in %s on line %d
bool(false)
===DONE===
