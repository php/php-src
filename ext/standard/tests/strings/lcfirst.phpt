--TEST--
lcfirst() function
--INI--
precision=14
--FILE--
<?php
/* Make a string's first character uppercase */

echo "#### Basic and Various operations ####\n";
$str_array = [
    "TesTing lcfirst.",
    "1.testing lcfirst",
    'HELLO wORLD',
    "\0",		// Null
    "-3",
    '-3.344',
    "NULL",
    "0",
    "TRUE",
    "1",
    "FALSE",
    " ",
    "     ",
    'b',		// single char
    "\t",		// escape sequences
    "12",
    "12twelve", // int + string
];

/* loop to test working of lcfirst with different values */
foreach ($str_array as $string) {
  var_dump( lcfirst($string) );
}

echo "\n--- Testing a longer and heredoc string ---\n";
$string = <<<EOD
Abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
@#$%^&**&^%$#@!~:())))((((&&&**%$###@@@!!!~~~~@###$%^&*
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
EOD;
var_dump(lcfirst($string));

echo "\n--- Nested lcfirst() ---\n";
var_dump(lcfirst(lcfirst("hello")));

echo "Done\n";
?>
--EXPECTF--
#### Basic and Various operations ####
string(16) "tesTing lcfirst."
string(17) "1.testing lcfirst"
string(11) "hELLO wORLD"
string(1) "%0"
string(2) "-3"
string(6) "-3.344"
string(4) "nULL"
string(1) "0"
string(4) "tRUE"
string(1) "1"
string(5) "fALSE"
string(1) " "
string(5) "     "
string(1) "b"
string(1) "	"
string(2) "12"
string(8) "12twelve"

--- Testing a longer and heredoc string ---
string(639) "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
@#$%^&**&^%$#@!~:())))((((&&&**%$###@@@!!!~~~~@###$%^&*
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"

--- Nested lcfirst() ---
string(5) "hello"
Done
