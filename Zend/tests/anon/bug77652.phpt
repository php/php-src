--TEST--
Bug #77652: Anonymous classes can lose their interface information
--FILE--
<?php

interface I {}
require __DIR__ . '/bug77652.inc';
$data = require __DIR__ . '/bug77652.inc';
print_r(class_implements($data['I']()));

?>
--EXPECT--
Array
(
    [I] => I
)
