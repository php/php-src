--TEST--
Test strrpos() function : basic functionality - with default arguments
--FILE--
<?php
echo "*** Testing strrpos() function: basic functionality ***\n";
$heredoc_str = <<<EOD
Hello, World
EOD;

echo "-- With default arguments --\n";
//regular string for haystack & needle
var_dump( strrpos("Hello, World", "Hello") );
var_dump( strrpos('Hello, World', "hello") );
var_dump( strrpos("Hello, World", 'World') );
var_dump( strrpos('Hello, World', 'WORLD') );

//single char for needle
var_dump( strrpos("Hello, World", "o") );
var_dump( strrpos("Hello, World", ",") );

//heredoc string for haystack & needle
var_dump( strrpos($heredoc_str, "Hello, World") );
var_dump( strrpos($heredoc_str, 'Hello') );
var_dump( strrpos($heredoc_str, $heredoc_str) );

echo "*** Done ***";
?>
--EXPECT--
*** Testing strrpos() function: basic functionality ***
-- With default arguments --
int(0)
bool(false)
int(7)
bool(false)
int(8)
int(5)
int(0)
int(0)
int(0)
*** Done ***
