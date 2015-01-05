--TEST--
Test preg_grep() function : error conditions - wrong arg types
--FILE--
<?php
/*
* proto array preg_grep(string regex, array input [, int flags])
* Function is implemented in ext/pcre/php_pcre.c
*/
error_reporting(E_ALL&~E_NOTICE);
/*
* Testing how preg_grep reacts to being passed the wrong type of input argument
*/
echo "*** Testing preg_grep() : error conditions ***\n";
$regex = '/[a-zA-Z]/';
$input = array('this is a string', array('this is', 'a subarray'),);
foreach($input as $value) {
    print "\nArg value is: $value\n";
    var_dump(preg_grep($regex, $value));
}
$value = new stdclass(); //Object
var_dump(preg_grep($regex, $value));
echo "Done";
?>
--EXPECTF--
*** Testing preg_grep() : error conditions ***

Arg value is: this is a string

Warning: preg_grep() expects parameter 2 to be array, string given in %spreg_grep_error2.php on line %d
NULL

Arg value is: Array
array(2) {
  [0]=>
  string(7) "this is"
  [1]=>
  string(10) "a subarray"
}

Warning: preg_grep() expects parameter 2 to be array, object given in %spreg_grep_error2.php on line %d
NULL
Done