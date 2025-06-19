--TEST--
GH-18597 (Heap-buffer-overflow in zend_alloc.c when assigning string with UTF-8 bytes)
--EXTENSIONS--
simplexml
--FILE--
<?php
$sx1 = new SimpleXMLElement("<root />");
$sx1->node[0] = 'node1';
$node = $sx1->node[0];

$node[0] = '��c';

$sx1->asXML(); // Depends on the available system encodings whether this fails or not, point is, it should not crash
echo "Done\n";
?>
--EXPECT--
Done
