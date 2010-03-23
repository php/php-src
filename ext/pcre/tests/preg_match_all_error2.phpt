--TEST--
Test preg_match_all() function : error conditions - wrong arg types
--FILE--
<?php
/*
* proto int preg_match_all(string pattern, string subject, array subpatterns [, int flags [, int offset]])
* Function is implemented in ext/pcre/php_pcre.c
*/
error_reporting(E_ALL&~E_NOTICE);
/*
* Testing how preg_match_all reacts to being passed the wrong type of input argument
*/
echo "*** Testing preg_match_all() : error conditions ***\n";
$regex = '/[a-zA-Z]/';
$value = new stdclass(); //Object
var_dump(preg_match_all($regex, $value, $matches));
var_dump($matches);
$input = array(array('this is', 'a subarray'), 'test',);
foreach($input as $value) {
    print "\nArg value is: $value\n";
    var_dump(preg_match_all($regex, $value, $matches));
    var_dump($matches);
}
echo "Done";
?>
--EXPECTF--
*** Testing preg_match_all() : error conditions ***

Warning: preg_match_all() expects parameter 2 to be string, object given in %spreg_match_all_error2.php on line %d
bool(false)
NULL

Arg value is: Array

Warning: preg_match_all() expects parameter 2 to be string, array given in %spreg_match_all_error2.php on line %d
bool(false)
NULL

Arg value is: test
int(4)
array(1) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "t"
    [1]=>
    string(1) "e"
    [2]=>
    string(1) "s"
    [3]=>
    string(1) "t"
  }
}
Done
