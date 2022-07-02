--TEST--
Test preg_match_all() function : error conditions - wrong arg types
--FILE--
<?php
/*
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing how preg_match_all reacts to being passed the wrong type of input argument
*/
echo "*** Testing preg_match_all() : error conditions ***\n";
$regex = '/[a-zA-Z]/';
$input = array(array('this is', 'a subarray'), 'test',);
foreach($input as $value) {
    @print "\nArg value is: $value\n";
    try {
        var_dump(preg_match_all($regex, $value, $matches));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($matches);
}
echo "Done";
?>
--EXPECT--
*** Testing preg_match_all() : error conditions ***

Arg value is: Array
preg_match_all(): Argument #2 ($subject) must be of type string, array given
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
