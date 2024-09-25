--TEST--
GH-16054 (Segmentation fault when resizing hash table iterator list while adding)
--FILE--
<?php
$multi_array = ['zero'];
$multi_array[] =& $multi_array;
$it = new RecursiveTreeIterator(new RecursiveArrayIterator($multi_array), 0);
$counter = 0;
foreach ($it as $k => $v) {
    if (++$counter > 200) break;
}
echo "ok\n";
?>
--EXPECT--
ok
