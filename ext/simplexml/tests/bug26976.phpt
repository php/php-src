--TEST--
Bug #26976 (Cannot access array elements using array indices)
--EXTENSIONS--
simplexml
--FILE--
<?php

$root = simplexml_load_string(
'<?xml version="1.0"?>
<root>
 <child>a</child>
 <child>b</child>
 <child>c</child>
 <child>d</child>
</root>
');

echo $root->child[0], "\n";
echo $root->child[1], "\n";
echo $root->child[2], "\n";
echo $root->child[3], "\n";

?>
--EXPECT--
a
b
c
d
