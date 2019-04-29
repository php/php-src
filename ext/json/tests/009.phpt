--TEST--
json_encode() with unpacked array with holes
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
$a = array(1, 2, 3, 'foo' => 'bar');
unset($a['foo']);

var_dump(json_encode($a));
echo "Done\n";
?>
--EXPECT--
string(7) "[1,2,3]"
Done

