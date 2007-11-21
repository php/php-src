--TEST--
Test preg_replace_callback() function : error 
--FILE--
<?php
/*
* proto string preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, count]])
* Function is implemented in ext/pcre/php_pcre.c
*/
echo "***Testing preg_replace_callback() : error conditions***\n";
//Zero arguments
echo "\n-- Testing preg_replace_callback() function with Zero arguments --\n";
var_dump(preg_replace_callback());
//Test preg_replace_callback() with one more than the expected number of arguments
echo "\n-- Testing preg_replace_callback() function with more than expected no. of arguments --\n";
$replacement = array('zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine');
function integer_word($matches) {
    global $replacement;
    return $replacement[$matches[0]];
}
$regex = '/\d/';
$subject = 'there are 7 words in this sentence.';
$limit = 10;
$extra_arg = 10;
var_dump(preg_replace_callback($regex, 'integer_word', $subject, $limit, $count, $extra_arg));
//Testing preg_replace_callback() with one less than the expected number of arguments
echo "\n-- Testing preg_replace_callback() function with less than expected no. of arguments --\n";
$regex = '/\d/';
var_dump(preg_replace_callback($regex, 'integer word'));
echo "Done";
?>
--EXPECTF--
***Testing preg_replace_callback() : error conditions***

-- Testing preg_replace_callback() function with Zero arguments --

Warning: Wrong parameter count for preg_replace_callback() in %spreg_replace_callback_error.php on line %d
NULL

-- Testing preg_replace_callback() function with more than expected no. of arguments --

Warning: Wrong parameter count for preg_replace_callback() in %spreg_replace_callback_error.php on line %d
NULL

-- Testing preg_replace_callback() function with less than expected no. of arguments --

Warning: Wrong parameter count for preg_replace_callback() in %spreg_replace_callback_error.php on line %d
NULL
Done
