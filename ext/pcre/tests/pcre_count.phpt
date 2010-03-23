--TEST--
preg_replace() fifth parameter - count
--FILE--
<?php
$regex = '/(([0-9a-z]+)-([0-9]+))-(([0-9]+)-([0-9]+))/';

$string= '1-2-3-4 a-2-3-4 1-a-3-4 1-2-a-4 1-2-3-a a-a-a-a 4-3-2-1 100-200-300-400-500-600-700-800';
$count = 0;
var_dump(preg_replace($regex, 'xxxx', $string, -1, $count));
var_dump($count);
//////////////////////////////////////////////////////

$regex = '/([a-z]+)/';

$string= 'Here must only number like 42 and 13 appear';
var_dump(preg_replace($regex, 'xxxx', $string, -1, $count));
var_dump($count);

////////////////////////////////////////////////////////
$regex = '~((V(I|1)(4|A)GR(4|A))|(V(I|1)C(0|O)D(I|1)(N|\/\\\/)))~i';

$string= 'Viagra V14GR4 Vicodin V1C0D1/\/ v1c0d1/|/';
var_dump(preg_replace($regex, '...', $string, -1, $count));
var_dump($count);
////////////////////////////////////////////////////////
$regex = '~((V(I|1)(4|A)GR(4|A))|(V(I|1)C(0|O)D(I|1)(N|\/\\\/)))~i';
$count = NULL;
$string= 'Viagra V14GR4 Vicodin V1C0D1/\/ v1c0d1/|/';
var_dump(preg_replace($regex, '...', $string, -1));
var_dump($count);
?>
--EXPECT--
string(56) "xxxx xxxx 1-a-3-4 1-2-a-4 1-2-3-a a-a-a-a xxxx xxxx-xxxx"
int(5)
string(41) "Hxxxx xxxx xxxx xxxx xxxx 42 xxxx 13 xxxx"
int(7)
string(25) "... ... ... ... v1c0d1/|/"
int(4)
string(25) "... ... ... ... v1c0d1/|/"
NULL
