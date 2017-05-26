--TEST--
Bug #72051 (The reference in CallbackFilterIterator doesn't work as expected)
--FILE--
<?php

$data = [
	[1,2]
];

$callbackTest = new CallbackFilterIterator(new ArrayIterator($data), function (&$current) {
	$current['message'] = 'Test message';
	return true;
});

$callbackTest->rewind();
$data = $callbackTest->current();
$callbackTest->next();
print_r($data);
?>
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [message] => Test message
)
