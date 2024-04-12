--TEST--
Test processing instruction character data manipulation
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root><?pi value?></root>');

$pi = $dom->documentElement->firstChild;

echo $pi->substringData(0, 3), "\n";
$pi->appendData('foobar');
echo $pi->textContent, "\n";
$pi->insertData(6, 'oooooo');
echo $pi->textContent, "\n";
$pi->deleteData(0, strlen('value'));
echo $pi->textContent, "\n";
$pi->replaceData(1, 8, 'oo');
echo $pi->textContent, "\n";

?>
--EXPECT--
val
valuefoobar
valuefoooooooobar
foooooooobar
foobar
