--TEST--
Bug #45280 (Reflection of instantiated COM classes causes PHP to crash)
--SKIPIF--
<?php
if (!extension_loaded("com_dotnet")){ echo "skip COM/.Net support not present"; }
?>
--FILE--
<?php
$dict = new COM("Scripting.Dictionary");

ob_start();
ReflectionObject::export($dict);
ob_get_clean();

echo 'done';
?>
--EXPECT--
done
