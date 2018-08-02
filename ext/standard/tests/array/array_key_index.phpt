--TEST--
array_key_first, array_key_last, array_key_index
--FILE--
<?php

function array_key_index_test($arr)
{
	reset($arr);
	next($arr); //skip to second parameter
	
	$key = array_key_first($arr);
	var_dump($key);
	echo current($arr) . "\n";
	echo "\n";
	
	
	$key = array_key_last($arr);
	var_dump($key);
	echo current($arr) . "\n";
	echo "\n";
	
	$key = array_key_index($arr, 1);
	
	var_dump($key);
	echo current($arr) . "\n";
	echo "\n";
	
	$key = array_key_index($arr, -3);
	
	var_dump($key);
	echo current($arr) . "\n";
	echo "\n";
	
	$key = array_key_index($arr, 3);
	var_dump($key);
	echo current($arr) . "\n";
	
	echo "\n\n";
}

function array_key_index_test_value($arr)
{
	reset($arr);
	next($arr); //skip to second parameter
	
	$value = array('value has not been written to');
	
	$key = array_key_first($arr, $value);
	var_dump($key);
	var_dump($value);
	echo current($arr) . "\n";
	echo "\n";
	
	
	$key = array_key_last($arr, $value);
	var_dump($key);
	var_dump($value);
	echo current($arr) . "\n";
	echo "\n";
	
	$key = array_key_index($arr, 1, $value);
	
	var_dump($key);
	var_dump($value);
	echo current($arr) . "\n";
	echo "\n";
	
	$key = array_key_index($arr, -3, $value);
	
	var_dump($key);
	var_dump($value);
	echo current($arr) . "\n";
	echo "\n";
	
	$key = array_key_index($arr, 3, $value);
	var_dump($key);
	var_dump($value);
	echo current($arr) . "\n";
	
	echo "\n\n";
}

echo "test with string array\n";
$arr = array(
	'one' => 'first',
	'two' => 'middle',
	'three' => 'last',
);
array_key_index_test($arr);
array_key_index_test_value($arr);

echo "test with numeric array\n";
$arr = array(
	1 => 'first',
	2 => 'middle',
	3 => 'last',
);
array_key_index_test($arr);
array_key_index_test_value($arr);

echo "test with mixed array\n";
$arr = array(
	1 => 'first',
	'second' => 'middle',
	3 => 'last',
);
array_key_index_test($arr);
array_key_index_test_value($arr);
?>
--EXPECT--
test with string array
string(3) "one"
middle

string(5) "three"
middle

string(3) "two"
middle

string(3) "one"
middle

NULL
middle


string(3) "one"
string(5) "first"
middle

string(5) "three"
string(4) "last"
middle

string(3) "two"
string(6) "middle"
middle

string(3) "one"
string(5) "first"
middle

NULL
string(5) "first"
middle


test with numeric array
int(1)
middle

int(3)
middle

int(2)
middle

int(1)
middle

NULL
middle


int(1)
string(5) "first"
middle

int(3)
string(4) "last"
middle

int(2)
string(6) "middle"
middle

int(1)
string(5) "first"
middle

NULL
string(5) "first"
middle


test with mixed array
int(1)
middle

int(3)
middle

string(6) "second"
middle

int(1)
middle

NULL
middle


int(1)
string(5) "first"
middle

int(3)
string(4) "last"
middle

string(6) "second"
string(6) "middle"
middle

int(1)
string(5) "first"
middle

NULL
string(5) "first"
middle
