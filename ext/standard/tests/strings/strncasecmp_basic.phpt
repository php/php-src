--TEST--
Test strncasecmp() function : basic functionality
--FILE--
<?php
echo "*** Testing strncasecmp() function: basic functionality ***\n";

echo "-- Testing strncasecmp() with single quoted string --\n";
var_dump( strncasecmp('Hello', 'Hello', 5) );  //expected: int(0)
var_dump( strncasecmp('Hello', 'Hi', 5) );  //expected: value < 0
var_dump( strncasecmp('Hi', 'Hello', 5) );  //expected: value > 0

echo "-- Testing strncasecmp() with double quoted string --\n";
var_dump( strncasecmp("Hello", "Hello", 5) );  //expected: int(0)
var_dump( strncasecmp("Hello", "Hi", 5) );  //expected: value < 0
var_dump( strncasecmp("Hi", "Hello", 5) );  //expected: value > 0

echo "-- Testing strncasecmp() with here-doc string --\n";
$str = <<<HEREDOC
Hello
HEREDOC;
var_dump( strncasecmp($str, "Hello", 5) );  //expected: int(0)
var_dump( strncasecmp($str, "Hi", 5) );  //expected: value < 0
var_dump( strncasecmp("Hi", $str, 5) );  //expected: value > 0

echo "*** Done ***";
?>
--EXPECTREGEX--
\*\*\* Testing strncasecmp\(\) function: basic functionality \*\*\*
-- Testing strncasecmp\(\) with single quoted string --
int\(0\)
int\(-[1-9][0-9]*\)
int\([1-9][0-9]*\)
-- Testing strncasecmp\(\) with double quoted string --
int\(0\)
int\(-[1-9][0-9]*\)
int\([1-9][0-9]*\)
-- Testing strncasecmp\(\) with here-doc string --
int\(0\)
int\(-[1-9][0-9]*\)
int\([1-9][0-9]*\)
\*\*\* Done \*\*\*
