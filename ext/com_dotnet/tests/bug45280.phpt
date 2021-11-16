--TEST--
Bug #45280 (Reflection of instantiated COM classes causes PHP to crash)
--EXTENSIONS--
com_dotnet
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
