--TEST--
Test preg_match_all() function : error conditions - bad regular expressions
--FILE--
<?php
/*
* proto int preg_match_all(string pattern, string subject, array subpatterns [, int flags [, int offset]])
* Function is implemented in ext/pcre/php_pcre.c
*/
error_reporting(E_ALL&~E_NOTICE);
/*
* Testing how preg_match_all reacts to being passed the wrong type of regex argument
*/
echo "*** Testing preg_match_all() : error conditions ***\n";
$regex_array = array('abcdef', //Regex without delimiter
'/[a-zA-Z]', //Regex without closing delimiter
'[a-zA-Z]/', //Regex without opening delimiter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[a-zA-Z]/', //Regex string
);
$subject = 'test';
foreach($regex_array as $regex_value) {
    print "\nArg value is $regex_value\n";
    var_dump(preg_match_all($regex_value, $subject, $matches1));
    var_dump($matches1);
}
$regex_value = new stdclass(); //Object
var_dump(preg_match_all($regex_value, $subject, $matches));
var_dump($matches);
?>
--EXPECTF--
*** Testing preg_match_all() : error conditions ***

Arg value is abcdef

Warning: preg_match_all(): Delimiter must not be alphanumeric or backslash in %spreg_match_all_error1.php on line %d
bool(false)
NULL

Arg value is /[a-zA-Z]

Warning: preg_match_all(): No ending delimiter '/' found in %spreg_match_all_error1.php on line %d
bool(false)
NULL

Arg value is [a-zA-Z]/

Warning: preg_match_all(): Unknown modifier '/' in %spreg_match_all_error1.php on line %d
bool(false)
NULL

Arg value is /[a-zA-Z]/F

Warning: preg_match_all(): Unknown modifier 'F' in %spreg_match_all_error1.php on line %d
bool(false)
NULL

Arg value is Array

Warning: preg_match_all() expects parameter 1 to be string, array given in %spreg_match_all_error1.php on line %d
bool(false)
NULL

Arg value is /[a-zA-Z]/
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

Warning: preg_match_all() expects parameter 1 to be string, object given in %spreg_match_all_error1.php on line %d
bool(false)
NULL
