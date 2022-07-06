--TEST--
Bug #45280 (Reflection of instantiated COM classes causes PHP to crash)
--SKIPIF--
<?php
if (!extension_loaded("com_dotnet")){ echo "skip COM/.Net support not present"; }
?>
--FILE--
<?php
$dict = new COM("Scripting.Dictionary");

$reflection = new ReflectionObject($dict);
ob_start();
echo $reflection;
ob_get_clean();

echo 'done';
?>
--EXPECT--
done
