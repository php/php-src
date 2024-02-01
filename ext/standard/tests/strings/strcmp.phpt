--TEST--
strcmp() function
--INI--
precision = 12
--FILE--
<?php
/* Compares two strings in case-sensitive manner */

echo "#### Basic and Possible operations ####";
/* creating an array of strings to be compared */
$arrays = array(
           array("a", "A", 'a', 'A', chr(128), chr(255), chr(256)),
           array("acc", "Acc", 'ac', "accc", 'acd', "?acc", 'acc!', "$!acc", ";acc"),
           array("1", "0", 0, "-1", -1, "", TRUE, FALSE, "string"),
           array(10.5, 1.5, 9.5, 11.5, 100.5, 10.5E1, -10.5, 10, 0.5)
          );

/* loop through to go each and every element in an array
    and comparing the elements with one and other */
foreach($arrays as $str1_arr){
  echo "\n*** comparing the strings in an \n";
  print_r($str1_arr);
  for ($i=0; $i<count($str1_arr); $i++){
    echo "\nIteration $i\n";
    for($j=0; $j<count($str1_arr); $j++){
      echo "- strcmp of '$str1_arr[$i]' and '$str1_arr[$j]' is => ";
      var_dump(strcmp($str1_arr[$i], $str1_arr[$j]));
    }
  }
}



echo "\n#### Testing miscellaneous inputs ####\n";

echo "--- Testing objects ---\n";
/* we get "Recoverable fatal error: saying Object of class could not be converted
   to string" by default, when an object is passed instead of string.
The error can be  avoided by choosing the __toString magix method as follows: */

class string1 {
  function __toString() {
    return "Hello, world";
  }
}
$obj_string1 = new string1;

class string2 {
  function __toString() {
    return "Hello, world\0";
  }
}
$obj_string2 = new string2;

var_dump(strcmp("$obj_string1", "$obj_string2"));


echo "\n--- Testing arrays ---\n";
$str_arr = array("hello", "?world", "!$%**()%**[][[[&@#~!");
var_dump(strcmp("hello?world,!$%**()%**[][[[&@#~!", "$str_arr[1]"));
var_dump(strcmp("hello?world,!$%**()%**[][[[&@#~!", "$str_arr[2]"));


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
var_dump(strcmp($string, $string));
var_dump(strcmp($string, "xyz0123456789"));
var_dump(strcmp($string, "&&&"));

echo "\n--- Testing a heredoc null string ---\n";
$str = <<<EOD
EOD;
var_dump(strcmp($str, "\0"));
var_dump(strcmp($str, "0"));


echo "\n--- Testing simple and complex syntax strings ---\n";
$str = 'world';

/* Simple syntax */
var_dump(strcmp("Hello, world", "$str"));
var_dump(strcmp("Hello, world'S", "$str'S"));
var_dump(strcmp("Hello, worldS", "$strS"));

/* String with curly braces, complex syntax */
var_dump(strcmp("Hello, worldS", "${str}S"));
var_dump(strcmp("Hello, worldS", "{$str}S"));

echo "\n--- Testing binary safe and binary chars ---\n";
var_dump(strcmp("Hello\0world", "Hello"));
var_dump(strcmp("Hello\0world", "Helloworld"));
var_dump(strcmp("\x0", "\0"));
var_dump(strcmp("\000", "\0"));
var_dump(strcmp("\x00", ""));

echo "\n--- Comparing long float values ---\n";
/* Here two different outputs, which depends on the rounding value
   before converting to string. Here Precision = 12  */
var_dump(strcmp(10.55555555555555555555555555, 10.5555555556));   // int(0)
var_dump(strcmp(10.55555555555555555555555555, 10.555555556));    // int(-1)
var_dump(strcmp(10.55555555595555555555555555, 10.555555556));    // int(0)

echo "Done\n";
?>
--EXPECTF--
Deprecated: Using ${var} in strings is deprecated, use {$var} instead in %s on line %d
#### Basic and Possible operations ####
*** comparing the strings in an 
Array
(
    [0] => a
    [1] => A
    [2] => a
    [3] => A
    [4] => €
    [5] => ÿ
    [6] => %0
)

Iteration 0
- strcmp of 'a' and 'a' is => int(0)
- strcmp of 'a' and 'A' is => int(%d)
- strcmp of 'a' and 'a' is => int(0)
- strcmp of 'a' and 'A' is => int(%d)
- strcmp of 'a' and '€' is => int(-%d)
- strcmp of 'a' and 'ÿ' is => int(-%d)
- strcmp of 'a' and '%0' is => int(%d)

Iteration 1
- strcmp of 'A' and 'a' is => int(-%d)
- strcmp of 'A' and 'A' is => int(0)
- strcmp of 'A' and 'a' is => int(-%d)
- strcmp of 'A' and 'A' is => int(0)
- strcmp of 'A' and '€' is => int(-%d)
- strcmp of 'A' and 'ÿ' is => int(-%d)
- strcmp of 'A' and '%0' is => int(%d)

Iteration 2
- strcmp of 'a' and 'a' is => int(0)
- strcmp of 'a' and 'A' is => int(%d)
- strcmp of 'a' and 'a' is => int(0)
- strcmp of 'a' and 'A' is => int(%d)
- strcmp of 'a' and '€' is => int(-%d)
- strcmp of 'a' and 'ÿ' is => int(-%d)
- strcmp of 'a' and '%0' is => int(%d)

Iteration 3
- strcmp of 'A' and 'a' is => int(-%d)
- strcmp of 'A' and 'A' is => int(0)
- strcmp of 'A' and 'a' is => int(-%d)
- strcmp of 'A' and 'A' is => int(0)
- strcmp of 'A' and '€' is => int(-%d)
- strcmp of 'A' and 'ÿ' is => int(-%d)
- strcmp of 'A' and '%0' is => int(%d)

Iteration 4
- strcmp of '€' and 'a' is => int(%d)
- strcmp of '€' and 'A' is => int(%d)
- strcmp of '€' and 'a' is => int(%d)
- strcmp of '€' and 'A' is => int(%d)
- strcmp of '€' and '€' is => int(0)
- strcmp of '€' and 'ÿ' is => int(-%d)
- strcmp of '€' and '%0' is => int(%d)

Iteration 5
- strcmp of 'ÿ' and 'a' is => int(%d)
- strcmp of 'ÿ' and 'A' is => int(%d)
- strcmp of 'ÿ' and 'a' is => int(%d)
- strcmp of 'ÿ' and 'A' is => int(%d)
- strcmp of 'ÿ' and '€' is => int(%d)
- strcmp of 'ÿ' and 'ÿ' is => int(0)
- strcmp of 'ÿ' and '%0' is => int(%d)

Iteration 6
- strcmp of '%0' and 'a' is => int(-%d)
- strcmp of '%0' and 'A' is => int(-%d)
- strcmp of '%0' and 'a' is => int(-%d)
- strcmp of '%0' and 'A' is => int(-%d)
- strcmp of '%0' and '€' is => int(-%d)
- strcmp of '%0' and 'ÿ' is => int(-%d)
- strcmp of '%0' and '%0' is => int(0)

*** comparing the strings in an 
Array
(
    [0] => acc
    [1] => Acc
    [2] => ac
    [3] => accc
    [4] => acd
    [5] => ?acc
    [6] => acc!
    [7] => $!acc
    [8] => ;acc
)

Iteration 0
- strcmp of 'acc' and 'acc' is => int(0)
- strcmp of 'acc' and 'Acc' is => int(%d)
- strcmp of 'acc' and 'ac' is => int(%d)
- strcmp of 'acc' and 'accc' is => int(-%d)
- strcmp of 'acc' and 'acd' is => int(-%d)
- strcmp of 'acc' and '?acc' is => int(%d)
- strcmp of 'acc' and 'acc!' is => int(-%d)
- strcmp of 'acc' and '$!acc' is => int(%d)
- strcmp of 'acc' and ';acc' is => int(%d)

Iteration 1
- strcmp of 'Acc' and 'acc' is => int(-%d)
- strcmp of 'Acc' and 'Acc' is => int(0)
- strcmp of 'Acc' and 'ac' is => int(-%d)
- strcmp of 'Acc' and 'accc' is => int(-%d)
- strcmp of 'Acc' and 'acd' is => int(-%d)
- strcmp of 'Acc' and '?acc' is => int(%d)
- strcmp of 'Acc' and 'acc!' is => int(-%d)
- strcmp of 'Acc' and '$!acc' is => int(%d)
- strcmp of 'Acc' and ';acc' is => int(%d)

Iteration 2
- strcmp of 'ac' and 'acc' is => int(-%d)
- strcmp of 'ac' and 'Acc' is => int(%d)
- strcmp of 'ac' and 'ac' is => int(0)
- strcmp of 'ac' and 'accc' is => int(-%d)
- strcmp of 'ac' and 'acd' is => int(-%d)
- strcmp of 'ac' and '?acc' is => int(%d)
- strcmp of 'ac' and 'acc!' is => int(-%d)
- strcmp of 'ac' and '$!acc' is => int(%d)
- strcmp of 'ac' and ';acc' is => int(%d)

Iteration 3
- strcmp of 'accc' and 'acc' is => int(%d)
- strcmp of 'accc' and 'Acc' is => int(%d)
- strcmp of 'accc' and 'ac' is => int(%d)
- strcmp of 'accc' and 'accc' is => int(0)
- strcmp of 'accc' and 'acd' is => int(-%d)
- strcmp of 'accc' and '?acc' is => int(%d)
- strcmp of 'accc' and 'acc!' is => int(%d)
- strcmp of 'accc' and '$!acc' is => int(%d)
- strcmp of 'accc' and ';acc' is => int(%d)

Iteration 4
- strcmp of 'acd' and 'acc' is => int(%d)
- strcmp of 'acd' and 'Acc' is => int(%d)
- strcmp of 'acd' and 'ac' is => int(%d)
- strcmp of 'acd' and 'accc' is => int(%d)
- strcmp of 'acd' and 'acd' is => int(0)
- strcmp of 'acd' and '?acc' is => int(%d)
- strcmp of 'acd' and 'acc!' is => int(%d)
- strcmp of 'acd' and '$!acc' is => int(%d)
- strcmp of 'acd' and ';acc' is => int(%d)

Iteration 5
- strcmp of '?acc' and 'acc' is => int(-%d)
- strcmp of '?acc' and 'Acc' is => int(-%d)
- strcmp of '?acc' and 'ac' is => int(-%d)
- strcmp of '?acc' and 'accc' is => int(-%d)
- strcmp of '?acc' and 'acd' is => int(-%d)
- strcmp of '?acc' and '?acc' is => int(0)
- strcmp of '?acc' and 'acc!' is => int(-%d)
- strcmp of '?acc' and '$!acc' is => int(%d)
- strcmp of '?acc' and ';acc' is => int(%d)

Iteration 6
- strcmp of 'acc!' and 'acc' is => int(%d)
- strcmp of 'acc!' and 'Acc' is => int(%d)
- strcmp of 'acc!' and 'ac' is => int(%d)
- strcmp of 'acc!' and 'accc' is => int(-%d)
- strcmp of 'acc!' and 'acd' is => int(-%d)
- strcmp of 'acc!' and '?acc' is => int(%d)
- strcmp of 'acc!' and 'acc!' is => int(0)
- strcmp of 'acc!' and '$!acc' is => int(%d)
- strcmp of 'acc!' and ';acc' is => int(%d)

Iteration 7
- strcmp of '$!acc' and 'acc' is => int(-%d)
- strcmp of '$!acc' and 'Acc' is => int(-%d)
- strcmp of '$!acc' and 'ac' is => int(-%d)
- strcmp of '$!acc' and 'accc' is => int(-%d)
- strcmp of '$!acc' and 'acd' is => int(-%d)
- strcmp of '$!acc' and '?acc' is => int(-%d)
- strcmp of '$!acc' and 'acc!' is => int(-%d)
- strcmp of '$!acc' and '$!acc' is => int(0)
- strcmp of '$!acc' and ';acc' is => int(-%d)

Iteration 8
- strcmp of ';acc' and 'acc' is => int(-%d)
- strcmp of ';acc' and 'Acc' is => int(-%d)
- strcmp of ';acc' and 'ac' is => int(-%d)
- strcmp of ';acc' and 'accc' is => int(-%d)
- strcmp of ';acc' and 'acd' is => int(-%d)
- strcmp of ';acc' and '?acc' is => int(-%d)
- strcmp of ';acc' and 'acc!' is => int(-%d)
- strcmp of ';acc' and '$!acc' is => int(%d)
- strcmp of ';acc' and ';acc' is => int(0)

*** comparing the strings in an 
Array
(
    [0] => 1
    [1] => 0
    [2] => 0
    [3] => -1
    [4] => -1
    [5] => 
    [6] => 1
    [7] => 
    [8] => string
)

Iteration 0
- strcmp of '1' and '1' is => int(0)
- strcmp of '1' and '0' is => int(%d)
- strcmp of '1' and '0' is => int(%d)
- strcmp of '1' and '-1' is => int(%d)
- strcmp of '1' and '-1' is => int(%d)
- strcmp of '1' and '' is => int(%d)
- strcmp of '1' and '1' is => int(0)
- strcmp of '1' and '' is => int(%d)
- strcmp of '1' and 'string' is => int(-%d)

Iteration 1
- strcmp of '0' and '1' is => int(-%d)
- strcmp of '0' and '0' is => int(0)
- strcmp of '0' and '0' is => int(0)
- strcmp of '0' and '-1' is => int(%d)
- strcmp of '0' and '-1' is => int(%d)
- strcmp of '0' and '' is => int(%d)
- strcmp of '0' and '1' is => int(-%d)
- strcmp of '0' and '' is => int(%d)
- strcmp of '0' and 'string' is => int(-%d)

Iteration 2
- strcmp of '0' and '1' is => int(-%d)
- strcmp of '0' and '0' is => int(0)
- strcmp of '0' and '0' is => int(0)
- strcmp of '0' and '-1' is => int(%d)
- strcmp of '0' and '-1' is => int(%d)
- strcmp of '0' and '' is => int(%d)
- strcmp of '0' and '1' is => int(-%d)
- strcmp of '0' and '' is => int(%d)
- strcmp of '0' and 'string' is => int(-%d)

Iteration 3
- strcmp of '-1' and '1' is => int(-%d)
- strcmp of '-1' and '0' is => int(-%d)
- strcmp of '-1' and '0' is => int(-%d)
- strcmp of '-1' and '-1' is => int(0)
- strcmp of '-1' and '-1' is => int(0)
- strcmp of '-1' and '' is => int(%d)
- strcmp of '-1' and '1' is => int(-%d)
- strcmp of '-1' and '' is => int(%d)
- strcmp of '-1' and 'string' is => int(-%d)

Iteration 4
- strcmp of '-1' and '1' is => int(-%d)
- strcmp of '-1' and '0' is => int(-%d)
- strcmp of '-1' and '0' is => int(-%d)
- strcmp of '-1' and '-1' is => int(0)
- strcmp of '-1' and '-1' is => int(0)
- strcmp of '-1' and '' is => int(%d)
- strcmp of '-1' and '1' is => int(-%d)
- strcmp of '-1' and '' is => int(%d)
- strcmp of '-1' and 'string' is => int(-%d)

Iteration 5
- strcmp of '' and '1' is => int(-%d)
- strcmp of '' and '0' is => int(-%d)
- strcmp of '' and '0' is => int(-%d)
- strcmp of '' and '-1' is => int(-%d)
- strcmp of '' and '-1' is => int(-%d)
- strcmp of '' and '' is => int(0)
- strcmp of '' and '1' is => int(-%d)
- strcmp of '' and '' is => int(0)
- strcmp of '' and 'string' is => int(-%d)

Iteration 6
- strcmp of '1' and '1' is => int(0)
- strcmp of '1' and '0' is => int(%d)
- strcmp of '1' and '0' is => int(%d)
- strcmp of '1' and '-1' is => int(%d)
- strcmp of '1' and '-1' is => int(%d)
- strcmp of '1' and '' is => int(%d)
- strcmp of '1' and '1' is => int(0)
- strcmp of '1' and '' is => int(%d)
- strcmp of '1' and 'string' is => int(-%d)

Iteration 7
- strcmp of '' and '1' is => int(-%d)
- strcmp of '' and '0' is => int(-%d)
- strcmp of '' and '0' is => int(-%d)
- strcmp of '' and '-1' is => int(-%d)
- strcmp of '' and '-1' is => int(-%d)
- strcmp of '' and '' is => int(0)
- strcmp of '' and '1' is => int(-%d)
- strcmp of '' and '' is => int(0)
- strcmp of '' and 'string' is => int(-%d)

Iteration 8
- strcmp of 'string' and '1' is => int(%d)
- strcmp of 'string' and '0' is => int(%d)
- strcmp of 'string' and '0' is => int(%d)
- strcmp of 'string' and '-1' is => int(%d)
- strcmp of 'string' and '-1' is => int(%d)
- strcmp of 'string' and '' is => int(%d)
- strcmp of 'string' and '1' is => int(%d)
- strcmp of 'string' and '' is => int(%d)
- strcmp of 'string' and 'string' is => int(0)

*** comparing the strings in an 
Array
(
    [0] => 10.5
    [1] => 1.5
    [2] => 9.5
    [3] => 11.5
    [4] => 100.5
    [5] => 105
    [6] => -10.5
    [7] => 10
    [8] => 0.5
)

Iteration 0
- strcmp of '10.5' and '10.5' is => int(0)
- strcmp of '10.5' and '1.5' is => int(%d)
- strcmp of '10.5' and '9.5' is => int(-%d)
- strcmp of '10.5' and '11.5' is => int(-%d)
- strcmp of '10.5' and '100.5' is => int(-%d)
- strcmp of '10.5' and '105' is => int(-%d)
- strcmp of '10.5' and '-10.5' is => int(%d)
- strcmp of '10.5' and '10' is => int(%d)
- strcmp of '10.5' and '0.5' is => int(%d)

Iteration 1
- strcmp of '1.5' and '10.5' is => int(-%d)
- strcmp of '1.5' and '1.5' is => int(0)
- strcmp of '1.5' and '9.5' is => int(-%d)
- strcmp of '1.5' and '11.5' is => int(-%d)
- strcmp of '1.5' and '100.5' is => int(-%d)
- strcmp of '1.5' and '105' is => int(-%d)
- strcmp of '1.5' and '-10.5' is => int(%d)
- strcmp of '1.5' and '10' is => int(-%d)
- strcmp of '1.5' and '0.5' is => int(%d)

Iteration 2
- strcmp of '9.5' and '10.5' is => int(%d)
- strcmp of '9.5' and '1.5' is => int(%d)
- strcmp of '9.5' and '9.5' is => int(0)
- strcmp of '9.5' and '11.5' is => int(%d)
- strcmp of '9.5' and '100.5' is => int(%d)
- strcmp of '9.5' and '105' is => int(%d)
- strcmp of '9.5' and '-10.5' is => int(%d)
- strcmp of '9.5' and '10' is => int(%d)
- strcmp of '9.5' and '0.5' is => int(%d)

Iteration 3
- strcmp of '11.5' and '10.5' is => int(%d)
- strcmp of '11.5' and '1.5' is => int(%d)
- strcmp of '11.5' and '9.5' is => int(-%d)
- strcmp of '11.5' and '11.5' is => int(0)
- strcmp of '11.5' and '100.5' is => int(%d)
- strcmp of '11.5' and '105' is => int(%d)
- strcmp of '11.5' and '-10.5' is => int(%d)
- strcmp of '11.5' and '10' is => int(%d)
- strcmp of '11.5' and '0.5' is => int(%d)

Iteration 4
- strcmp of '100.5' and '10.5' is => int(%d)
- strcmp of '100.5' and '1.5' is => int(%d)
- strcmp of '100.5' and '9.5' is => int(-%d)
- strcmp of '100.5' and '11.5' is => int(-%d)
- strcmp of '100.5' and '100.5' is => int(0)
- strcmp of '100.5' and '105' is => int(-%d)
- strcmp of '100.5' and '-10.5' is => int(%d)
- strcmp of '100.5' and '10' is => int(%d)
- strcmp of '100.5' and '0.5' is => int(%d)

Iteration 5
- strcmp of '105' and '10.5' is => int(%d)
- strcmp of '105' and '1.5' is => int(%d)
- strcmp of '105' and '9.5' is => int(-%d)
- strcmp of '105' and '11.5' is => int(-%d)
- strcmp of '105' and '100.5' is => int(%d)
- strcmp of '105' and '105' is => int(0)
- strcmp of '105' and '-10.5' is => int(%d)
- strcmp of '105' and '10' is => int(%d)
- strcmp of '105' and '0.5' is => int(%d)

Iteration 6
- strcmp of '-10.5' and '10.5' is => int(-%d)
- strcmp of '-10.5' and '1.5' is => int(-%d)
- strcmp of '-10.5' and '9.5' is => int(-%d)
- strcmp of '-10.5' and '11.5' is => int(-%d)
- strcmp of '-10.5' and '100.5' is => int(-%d)
- strcmp of '-10.5' and '105' is => int(-%d)
- strcmp of '-10.5' and '-10.5' is => int(0)
- strcmp of '-10.5' and '10' is => int(-%d)
- strcmp of '-10.5' and '0.5' is => int(-%d)

Iteration 7
- strcmp of '10' and '10.5' is => int(-%d)
- strcmp of '10' and '1.5' is => int(%d)
- strcmp of '10' and '9.5' is => int(-%d)
- strcmp of '10' and '11.5' is => int(-%d)
- strcmp of '10' and '100.5' is => int(-%d)
- strcmp of '10' and '105' is => int(-%d)
- strcmp of '10' and '-10.5' is => int(%d)
- strcmp of '10' and '10' is => int(0)
- strcmp of '10' and '0.5' is => int(%d)

Iteration 8
- strcmp of '0.5' and '10.5' is => int(-%d)
- strcmp of '0.5' and '1.5' is => int(-%d)
- strcmp of '0.5' and '9.5' is => int(-%d)
- strcmp of '0.5' and '11.5' is => int(-%d)
- strcmp of '0.5' and '100.5' is => int(-%d)
- strcmp of '0.5' and '105' is => int(-%d)
- strcmp of '0.5' and '-10.5' is => int(%d)
- strcmp of '0.5' and '10' is => int(-%d)
- strcmp of '0.5' and '0.5' is => int(0)

#### Testing miscellaneous inputs ####
--- Testing objects ---
int(-%d)

--- Testing arrays ---
int(%d)
int(%d)

--- Testing a longer and heredoc string ---
int(0)
int(-%d)
int(%d)

--- Testing a heredoc null string ---
int(-%d)
int(-%d)

--- Testing simple and complex syntax strings ---
int(-%d)
int(-%d)

Warning: Undefined variable $strS in %s on line %d
int(%d)
int(-%d)
int(-%d)

--- Testing binary safe and binary chars ---
int(%d)
int(-%d)
int(0)
int(0)
int(%d)

--- Comparing long float values ---
int(0)
int(-%d)
int(0)
Done
