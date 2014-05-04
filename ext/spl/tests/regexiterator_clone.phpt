--TEST--
SPL: RegexIterator clone
--FILE--
<?php

$testData = array("Test1", "something", "Test2");

$it1 = new RegexIterator(new ArrayIterator($testData), '/Test.*/');
$it1->rewind();
$it2=clone $it1;
$it2->next();
print($it2->current().PHP_EOL);
print($it2->getRegex().PHP_EOL);
?>
--EXPECT--
Test2
/Test.*/
