--TEST--
Test eregi() function : basic functionality - confirm case insensitivity
--FILE--
<?php
/* Prototype  : proto int eregi(string pattern, string string [, array registers])
 * Description: Case-insensitive regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test basic funtionality of eregi()
 */

echo "*** Testing eregi() : basic functionality ***\n";
$string = <<<END
UPPERCASE WORDS
lowercase words
MIxED CaSe woRdS
END;

var_dump(eregi('words', $string, $match1));
var_dump($match1);

var_dump(eregi('[[:lower:]]+[[:space:]]case', $string, $match2));  //character class lower should just match [a-z] but in case insensitive search matches [a-zA-Z]
var_dump($match2);
echo "Done";
?>
--EXPECTF--
*** Testing eregi() : basic functionality ***
int(5)
array(1) {
  [0]=>
  string(5) "WORDS"
}
int(10)
array(1) {
  [0]=>
  string(10) "MIxED CaSe"
}
Done