--TEST--
"ucfirst()" function
--INI--
precision=14
--FILE--
<?php
/* Make a string's first character uppercase */

echo "#### Basic and Various operations ####\n";
$str_array = array(
            "testing ucfirst.",
            "1.testing ucfirst",
            "hELLO wORLD",
            'hELLO wORLD',
                    "\0",		// Null
                    "\x00",		// Hex Null
                    "\x000",
                    "abcd",		// double quoted string
                    'xyz',		// single quoted string
                    "-3",
                    -3,
                    '-3.344',
                    -3.344,
                    "NULL",
                    "0",
                    0,
                    TRUE,		// bool type
                    "TRUE",
                    "1",
                    1,
                    1.234444,
                    FALSE,
                    "FALSE",
                    " ",
                    "     ",
                    'b',		// single char
                    '\t',		// escape sequences
                    "\t",
                    "12",
                    "12twelve",		// int + string
              );
/* loop to test working of ucfirst with different values */
foreach ($str_array as $string) {
  var_dump( ucfirst($string) );
}



echo "\n#### Testing miscellaneous inputs ####\n";

echo "\n--- Testing objects ---\n";
/* we get "Recoverable fatal error: saying Object of class could not be converted
        to string" by default when an object is passed instead of string:
The error can be  avoided by choosing the __toString magix method as follows: */

class StringCapable {
  function __toString() {
    return "hello, world";
  }
}
$obj_string = new StringCapable;

var_dump(ucfirst("$obj_string"));


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

echo "\n--- Testing a heredoc null string ---\n";
$str = <<<EOD
EOD;
var_dump(ucfirst($str));


echo "\n--- Testing simple and complex syntax strings ---\n";
$str = 'world';

/* Simple syntax */
var_dump(ucfirst("$str"));
var_dump(ucfirst("$str'S"));
var_dump(ucfirst("$strS"));

/* String with curly braces, complex syntax */
var_dump(ucfirst("${str}S"));
var_dump(ucfirst("{$str}S"));

echo "\n--- Nested ucfirst() ---\n";
var_dump(ucfirst(ucfirst("hello")));

echo "Done\n";
?>
--EXPECTF--
Deprecated: Using ${var} in strings is deprecated, use {$var} instead in %s on line %d
#### Basic and Various operations ####
string(16) "Testing ucfirst."
string(17) "1.testing ucfirst"
string(11) "HELLO wORLD"
string(11) "HELLO wORLD"
string(1) "%0"
string(1) "%0"
string(2) "%00"
string(4) "Abcd"
string(3) "Xyz"
string(2) "-3"
string(2) "-3"
string(6) "-3.344"
string(6) "-3.344"
string(4) "NULL"
string(1) "0"
string(1) "0"
string(1) "1"
string(4) "TRUE"
string(1) "1"
string(1) "1"
string(8) "1.234444"
string(0) ""
string(5) "FALSE"
string(1) " "
string(5) "     "
string(1) "B"
string(2) "\t"
string(1) "	"
string(2) "12"
string(8) "12twelve"

#### Testing miscellaneous inputs ####

--- Testing objects ---
string(12) "Hello, world"

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

--- Testing a heredoc null string ---
string(0) ""

--- Testing simple and complex syntax strings ---
string(5) "World"
string(7) "World'S"

Warning: Undefined variable $strS in %s on line %d
string(0) ""
string(6) "WorldS"
string(6) "WorldS"

--- Nested ucfirst() ---
string(5) "Hello"
Done
