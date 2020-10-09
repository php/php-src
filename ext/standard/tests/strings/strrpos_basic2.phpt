--TEST--
Test strrpos() function : basic functionality - with all arguments
--FILE--
<?php
echo "*** Testing strrpos() function: basic functionality ***\n";
$heredoc_str = <<<EOD
Hello, World
EOD;

echo "-- With all arguments --\n";
//regular string for haystack & needle, with various offsets
var_dump( strrpos("Hello, World", "Hello", 0) );
var_dump( strrpos("Hello, World", 'Hello', 1) );
var_dump( strrpos('Hello, World', 'World', 1) );
var_dump( strrpos('Hello, World', "World", 5) );

//heredoc string for haystack & needle, with various offsets
var_dump( strrpos($heredoc_str, "Hello, World", 0) );
var_dump( strrpos($heredoc_str, 'Hello', 0) );
var_dump( strrpos($heredoc_str, 'Hello', 1) );
var_dump( strrpos($heredoc_str, $heredoc_str, 0) );
var_dump( strrpos($heredoc_str, $heredoc_str, 1) );

//various offsets
var_dump( strrpos("Hello, World", "o", 3) );
var_dump( strrpos("Hello, World", "o", 6) );
var_dump( strrpos("Hello, World", "o", 10) );
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrpos() function: basic functionality ***
-- With all arguments --
int(0)
bool(false)
int(7)
int(7)
int(0)
int(0)
bool(false)
int(0)
bool(false)
int(8)
int(8)
bool(false)
*** Done ***
