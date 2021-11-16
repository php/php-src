--TEST--
Bug #79248 (Traversing empty VT_ARRAY throws com_exception)
--EXTENSIONS--
com_dotnet
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
