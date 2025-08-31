--TEST--
Test preg_replace_callback() function : basic functionality
--FILE--
<?php
/*
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Basic test for preg_replace_callback
*/
$replacement = array('zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine'); //array will have the default key values (0-9) and each value is the corresponding key written in words.
function integer_word($matches) {
    global $replacement;
    return $replacement[$matches[0]]; //all examples will be looking for an integer value, if one is found it will be stored in $matches[0] which corresponds to a key in the $replacements array

}
$subject1 = 'there are 7 words in this sentence.';
$new_subject1 = preg_replace_callback('/\d/', "integer_word", $subject1);
print "$new_subject1 \n";
$subject2 = '1 2 3 4 is now written in words';
$new_subject2 = preg_replace_callback('/\d/', "integer_word", $subject2, 3); //limits to three replacements
print "$new_subject2 \n";
$subject3 = 'there are no numbers in this string';
$new_subject3 = preg_replace_callback('/\d/', "integer_word", $subject3, 5, $count); //limites to five replacements and counts the number of replacements made ands stores in $count variable
print "$new_subject3 \n";
print $count;
?>
--EXPECT--
there are seven words in this sentence. 
one two three 4 is now written in words 
there are no numbers in this string 
0
