--TEST--
list() with keys, evaluation order #2
--FILE--
<?php

// All the following should print 'a' then 'b'

list($a, $b) = ['a', 'b'];
var_dump($a);
var_dump($b);

list(0 => $a, 1 => $b) = ['a', 'b'];
var_dump($a);
var_dump($b);

list(1 => $b, 0 => $a) = ['a', 'b'];
var_dump($a);
var_dump($b);

$arr = [];
list($arr[], $arr[]) = ['a', 'b'];
var_dump($arr[0]);
var_dump($arr[1]);

$arr = [];
list(0 => $arr[], 1 => $arr[]) = ['a', 'b'];
var_dump($arr[0]);
var_dump($arr[1]);

$arr = [];
list(1 => $arr[], 0 => $arr[]) = ['b', 'a'];
var_dump($arr[0]);
var_dump($arr[1]);

$arr = [];
list(list(1 => $arr[], 0 => $arr[])) = [['b', 'a']];
var_dump($arr[0]);
var_dump($arr[1]);

$arr = [];
list('key1' => $arr[], 'key2' => $arr[]) = ['key2' => 'b', 'key1' => 'a'];
var_dump($arr[0]);
var_dump($arr[1]);

// This should print 'foo'
$a = 0;
list($a => $a) = ['foo', 'bar'];
var_dump($a);

// This should print 'bar' then 'foo'
$a = 0;
$b = 1;
list($b => $a, $a => $c) = ['bar' => 'foo', 1 => 'bar'];
var_dump($a);
var_dump($c);

?>
--EXPECT--
string(1) "a"
string(1) "b"
string(1) "a"
string(1) "b"
string(1) "a"
string(1) "b"
string(1) "a"
string(1) "b"
string(1) "a"
string(1) "b"
string(1) "a"
string(1) "b"
string(1) "a"
string(1) "b"
string(1) "a"
string(1) "b"
string(3) "foo"
string(3) "bar"
string(3) "foo"
