--TEST--
search_array and in_array (including bug 13567)
--FILE--
<?php

$arr1 = array('a','b','c');
$arr2 = array();
$arr3 = array('c','key'=>'d');
$arr4 = array("a\0b"=>'e','key'=>'d', 'f');

$tests = <<<TESTS
FALSE === in_array(123, \$arr1)
FALSE === array_search(123, \$arr1)
TRUE  === in_array('a', \$arr1)
0     === array_search('a', \$arr1)
"a\\0b"=== array_search('e', \$arr4)
'key'=== array_search('d', \$arr4)
TESTS;

include('tests/quicktester.inc');

--EXPECT--
OK
