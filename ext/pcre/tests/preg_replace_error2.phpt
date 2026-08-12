--TEST--
Test preg_replace() function : error conditions - wrong arg types
--FILE--
<?php
/*
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing how preg_replace reacts to being passed the wrong type of replacement argument
*/
$regex = '/[a-zA-Z]/';
$replace = array('this is a string', array('this is', 'a subarray'),);
$subject = 'test';
foreach($replace as $value) {
    try {
        var_dump(preg_replace($regex, $value, $subject));
    } catch (TypeError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
$value = new stdclass(); //Object
try {
    var_dump(preg_replace($regex, $value, $subject));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
string(64) "this is a stringthis is a stringthis is a stringthis is a string"
TypeError: preg_replace(): Argument #1 ($pattern) must be of type array when argument #2 ($replacement) is an array, string given
TypeError: preg_replace(): Argument #2 ($replacement) must be of type array|string, stdClass given
