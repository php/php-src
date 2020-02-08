--TEST--
Bug #79248 (Traversing empty VT_ARRAY throws com_exception)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
--FILE--
<?php
$v = new variant([], VT_ARRAY);
foreach ($v as $el) {
    var_dump($el);
}
echo "done\n";
?>
--EXPECT--
done
