--TEST--
strlen() function
--INI--
precision = 12
--FILE--
<?php
/* returns the length of a given string */

$heredoc = <<<EOD
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

echo "#### Basic operations and  variations ####\n";
$strings = [
    'Hello, World' => 12,
    'Hello, World\0' => 14,
    '!!Hello, World' => 14,
    '??Hello, World' => 14,
    '$@#%^&*!~,.:;?' => 14,
    '\t' => 2,
    '23' => 2,
    '-1.2345' => 7,
    '' => 0,
    ' ' => 1,
    "\0" => 1,
    "\x000" => 2,
    "\xABC" => 2,
    "\0000" => 2,
    "0" => 1,
    "\t" => 1,
    "Hello, World\0" => 13,
    "Hello\0World" => 11,
    "Hello, World\n" => 13,
    "Hello, World\r" => 13,
    "Hello, World\t" => 13,
    "Hello, World\\" => 13,
    "              " => 14,
    chr(128).chr(234).chr(65).chr(255) => 4,
    $heredoc => 639,
];

/* loop through to find the length of each string */
foreach ($strings as $string => $len) {
    $computed = strlen($string);
    if ($computed === $len) {
        echo "CORRECT\n";
    } else {
        echo "Expected length $len got $computed";
    }
}

echo "\n--- strlen for long float values ---\n";
/* Here two different outputs, which depends on the rounding value
   before converting to string. Here Precision = 12  */
var_dump(strlen(10.55555555555555555555555555)); // len = 13
var_dump(strlen(10.55555555595555555555555555)); // len = 12

echo "Done\n";
?>
--EXPECT--
#### Basic operations and  variations ####
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT
CORRECT

--- strlen for long float values ---
int(13)
int(12)
Done
