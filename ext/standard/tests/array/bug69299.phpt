--TEST--
Bug #69299 (Regression in array_filter's $flag argument in PHP 7)
--FILE--
<?php
$toFilter = array('foo' => 'bar', 'fiz' => 'buz');
$filtered = array_filter($toFilter, function ($value, $key) {
	if ($value === 'buz'
		|| $key === 'foo'
	) {
		return false;
	}
	return true;
}, ARRAY_FILTER_USE_BOTH);
var_dump($filtered);
?>
--EXPECT--
array(0) {
}
