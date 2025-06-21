--TEST--
"ucfirst()" function
--INI--
precision=14
--FILE--
<?php
/* Make a string's first character uppercase */

echo "#### Basic and Various operations ####\n";
$str_array = [
    "testing ucfirst.",
    "1.testing ucfirst",
    'hELLO wORLD',
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

/* loop to test working of ucfirst with different values */
foreach ($str_array as $string) {
  var_dump( ucfirst($string) );
}

echo "\n--- Testing a longer and heredoc string ---\n";
$string = <<<EOD
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
@#$%^&**&^%$#@!~:())))((((&&&**%$###@@@!!!~~~~@###$%^&*
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
EOD;
var_dump(ucfirst($string));

echo "\n--- Nested ucfirst() ---\n";
var_dump(ucfirst(ucfirst("hello")));

echo "Done\n";
?>
--EXPECTF--
#### Basic and Various operations ####
string(16) "Testing ucfirst."
string(17) "1.testing ucfirst"
string(11) "HELLO wORLD"
string(1) "%0"
string(2) "-3"
string(6) "-3.344"
string(4) "NULL"
string(1) "0"
string(4) "TRUE"
string(1) "1"
string(5) "FALSE"
string(1) " "
string(5) "     "
string(1) "B"
string(1) "	"
string(2) "12"
string(8) "12twelve"

--- Testing a longer and heredoc string ---
string(639) "Abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
@#$%^&**&^%$#@!~:())))((((&&&**%$###@@@!!!~~~~@###$%^&*
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789"

--- Nested ucfirst() ---
string(5) "Hello"
Done
