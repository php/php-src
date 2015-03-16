--TEST--
Test token_get_all() function with semi-reserved grammar
--FILE--
<?php

$tokens = token_get_all('<?php
class Foo {
    const CONST = "const";
    public $public = "public";
    protected function function(){}
}

const CONST = "const"; // syntax error

function function(){}  // syntax error

');

// var_dump($tokens);

echo token_name($tokens[7][0]), PHP_EOL;
echo token_name($tokens[9][0]), PHP_EOL;
echo PHP_EOL;
echo token_name($tokens[16][0]), PHP_EOL;
echo token_name($tokens[18][0]), PHP_EOL;
echo PHP_EOL;
echo token_name($tokens[27][0]), PHP_EOL;
echo token_name($tokens[29][0]), PHP_EOL;
echo PHP_EOL;
echo token_name($tokens[37][0]), PHP_EOL;
echo token_name($tokens[39][0]), PHP_EOL;
echo PHP_EOL;
echo token_name($tokens[48][0]), PHP_EOL;
echo token_name($tokens[50][0]), PHP_EOL;

echo "\nDone\n";

--EXPECT--

T_CONST
T_STRING

T_PUBLIC
T_VARIABLE

T_FUNCTION
T_STRING

T_CONST
T_CONST

T_FUNCTION
T_FUNCTION

Done
