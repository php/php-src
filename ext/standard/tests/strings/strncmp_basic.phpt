--TEST--
Test strncmp() function : basic functionality
--FILE--
<?php
echo "*** Testing strncmp() function: basic functionality ***\n";

echo "-- Testing strncmp() with single quoted string --\n";
var_dump( strncmp('Hello', 'Hello', 5) );  //expected: int(0)
var_dump( strncmp('Hello', 'Hi', 5) );  //expected: value < 0
var_dump( strncmp('Hi', 'Hello', 5) );  //expected: value > 0

echo "-- Testing strncmp() with double quoted string --\n";
var_dump( strncmp("Hello", "Hello", 5) );  //expected: int(0)
var_dump( strncmp("Hello", "Hi", 5) );  //expected: value < 0
var_dump( strncmp("Hi", "Hello", 5) );  //expected: value > 0

echo "-- Testing strncmp() with here-doc string --\n";
$str = <<<HEREDOC
Hello
HEREDOC;
var_dump( strncmp($str, "Hello", 5) );  //expected: int(0)
var_dump( strncmp($str, "Hi", 5) );  //expected: value < 0
var_dump( strncmp("Hi", $str, 5) );  //expected: value > 0

echo "*** Done ***";
?>
--EXPECTREGEX--
\*\*\* Testing strncmp\(\) function: basic functionality \*\*\*
-- Testing strncmp\(\) with single quoted string --
int\(0\)
int\(-[1-9][0-9]*\)
int\([1-9][0-9]*\)
-- Testing strncmp\(\) with double quoted string --
int\(0\)
int\(-[1-9][0-9]*\)
int\([1-9][0-9]*\)
-- Testing strncmp\(\) with here-doc string --
int\(0\)
int\(-[1-9][0-9]*\)
int\([1-9][0-9]*\)
\*\*\* Done \*\*\*
