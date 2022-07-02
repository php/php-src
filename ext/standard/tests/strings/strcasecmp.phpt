--TEST--
strcasecmp() function
--INI--
precision = 12
--FILE--
<?php
/* Compares two strings in case-insensitive manner */

echo "#### Basic and Possible operations ####";
/* creating an array of strings to be compared */
$arrays = array(
           array("a", 'A', chr(128), chr(255), chr(256)),
           array("acc", "Acc", 'aC', "acCc", 'acd', "?acc", 'Acc!', "$!acc", ";acc"),
           array("1", "0", 0, "-1", -1, "", TRUE, true, FALSE, "string"),
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
      echo "- strcasecmp of '$str1_arr[$i]' and '$str1_arr[$j]' is => ";
      var_dump(strcasecmp($str1_arr[$i], $str1_arr[$j]));
    }
  }
}



echo "\n#### Testing miscellaneous inputs ####\n";

echo "--- Testing objects ---\n";
/* we get "Recoverable fatal error: saying Object of class could not be converted
   to string" by default when an object is passed instead of string.
The error can be  avoided by choosing the __toString magix method as follows: */

class string1 {
  function __toString() {
    return "Hello, world";
  }
}
$obj_string1 = new string1;

class string2 {
  function __toString() {
    return "hello, world\0";
  }
}
$obj_string2 = new string2;

var_dump(strcasecmp("$obj_string1", "$obj_string2"));


echo "\n--- Testing arrays ---\n";
$str_arr = array("hello", "?world", "!$%**()%**[][[[&@#~!");
var_dump(strcasecmp("hello?world,!$%**()%**[][[[&@#~!", "$str_arr[1]"));
var_dump(strcasecmp("hello?world,!$%**()%**[][[[&@#~!", "$str_arr[2]"));


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
var_dump(strcasecmp($string, $string));
var_dump(strcasecmp($string, "xyz0123456789"));
var_dump(strcasecmp($string, "&&&"));

echo "\n--- Testing a heredoc null string ---\n";
$str = <<<EOD
EOD;
var_dump(strcasecmp($str, "\0"));
var_dump(strcasecmp($str, "0"));


echo "\n--- Testing simple and complex syntax strings ---\n";
$str = 'world';

/* Simple syntax */
var_dump(strcasecmp("Hello, world", "$str"));
var_dump(strcasecmp("Hello, world'S", "$str'S"));
var_dump(strcasecmp("Hello, worldS", "$strS"));

/* String with curly braces, complex syntax */
var_dump(strcasecmp("Hello, worldS", "${str}S"));
var_dump(strcasecmp("Hello, worldS", "{$str}S"));

echo "\n--- Testing binary safe and binary chars ---\n";
var_dump(strcasecmp("Hello\0world", "Hello"));
var_dump(strcasecmp("Hello\0world", "Helloworld"));
var_dump(strcasecmp("\x0", "\0"));
var_dump(strcasecmp("\000", "\0"));
var_dump(strcasecmp("\x00", ""));

echo "\n--- Comparing long float values ---\n";
/* Here two different outputs, which depends on the rounding value
   before converting to string. Here Precision = 12  */
var_dump(strcasecmp(10.55555555555555555555555555, 10.5555555556));   // int(0)
var_dump(strcasecmp(10.55555555555555555555555555, 10.555555556));    // int(-1)
var_dump(strcasecmp(10.55555555595555555555555555, 10.555555556));    // int(0)

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
    [2] => €
    [3] => ÿ
    [4] => %0
)

Iteration 0
- strcasecmp of 'a' and 'a' is => int(0)
- strcasecmp of 'a' and 'A' is => int(0)
- strcasecmp of 'a' and '€' is => int(-%d)
- strcasecmp of 'a' and 'ÿ' is => int(-%d)
- strcasecmp of 'a' and '%0' is => int(%d)

Iteration 1
- strcasecmp of 'A' and 'a' is => int(0)
- strcasecmp of 'A' and 'A' is => int(0)
- strcasecmp of 'A' and '€' is => int(-%d)
- strcasecmp of 'A' and 'ÿ' is => int(-%d)
- strcasecmp of 'A' and '%0' is => int(%d)

Iteration 2
- strcasecmp of '€' and 'a' is => int(%d)
- strcasecmp of '€' and 'A' is => int(%d)
- strcasecmp of '€' and '€' is => int(0)
- strcasecmp of '€' and 'ÿ' is => int(-%d)
- strcasecmp of '€' and '%0' is => int(%d)

Iteration 3
- strcasecmp of 'ÿ' and 'a' is => int(%d)
- strcasecmp of 'ÿ' and 'A' is => int(%d)
- strcasecmp of 'ÿ' and '€' is => int(%d)
- strcasecmp of 'ÿ' and 'ÿ' is => int(0)
- strcasecmp of 'ÿ' and '%0' is => int(%d)

Iteration 4
- strcasecmp of '%0' and 'a' is => int(-%d)
- strcasecmp of '%0' and 'A' is => int(-%d)
- strcasecmp of '%0' and '€' is => int(-%d)
- strcasecmp of '%0' and 'ÿ' is => int(-%d)
- strcasecmp of '%0' and '%0' is => int(0)

*** comparing the strings in an 
Array
(
    [0] => acc
    [1] => Acc
    [2] => aC
    [3] => acCc
    [4] => acd
    [5] => ?acc
    [6] => Acc!
    [7] => $!acc
    [8] => ;acc
)

Iteration 0
- strcasecmp of 'acc' and 'acc' is => int(0)
- strcasecmp of 'acc' and 'Acc' is => int(0)
- strcasecmp of 'acc' and 'aC' is => int(%d)
- strcasecmp of 'acc' and 'acCc' is => int(-%d)
- strcasecmp of 'acc' and 'acd' is => int(-%d)
- strcasecmp of 'acc' and '?acc' is => int(%d)
- strcasecmp of 'acc' and 'Acc!' is => int(-%d)
- strcasecmp of 'acc' and '$!acc' is => int(%d)
- strcasecmp of 'acc' and ';acc' is => int(%d)

Iteration 1
- strcasecmp of 'Acc' and 'acc' is => int(0)
- strcasecmp of 'Acc' and 'Acc' is => int(0)
- strcasecmp of 'Acc' and 'aC' is => int(%d)
- strcasecmp of 'Acc' and 'acCc' is => int(-%d)
- strcasecmp of 'Acc' and 'acd' is => int(-%d)
- strcasecmp of 'Acc' and '?acc' is => int(%d)
- strcasecmp of 'Acc' and 'Acc!' is => int(-%d)
- strcasecmp of 'Acc' and '$!acc' is => int(%d)
- strcasecmp of 'Acc' and ';acc' is => int(%d)

Iteration 2
- strcasecmp of 'aC' and 'acc' is => int(-%d)
- strcasecmp of 'aC' and 'Acc' is => int(-%d)
- strcasecmp of 'aC' and 'aC' is => int(0)
- strcasecmp of 'aC' and 'acCc' is => int(-%d)
- strcasecmp of 'aC' and 'acd' is => int(-%d)
- strcasecmp of 'aC' and '?acc' is => int(%d)
- strcasecmp of 'aC' and 'Acc!' is => int(-%d)
- strcasecmp of 'aC' and '$!acc' is => int(%d)
- strcasecmp of 'aC' and ';acc' is => int(%d)

Iteration 3
- strcasecmp of 'acCc' and 'acc' is => int(%d)
- strcasecmp of 'acCc' and 'Acc' is => int(%d)
- strcasecmp of 'acCc' and 'aC' is => int(%d)
- strcasecmp of 'acCc' and 'acCc' is => int(0)
- strcasecmp of 'acCc' and 'acd' is => int(-%d)
- strcasecmp of 'acCc' and '?acc' is => int(%d)
- strcasecmp of 'acCc' and 'Acc!' is => int(%d)
- strcasecmp of 'acCc' and '$!acc' is => int(%d)
- strcasecmp of 'acCc' and ';acc' is => int(%d)

Iteration 4
- strcasecmp of 'acd' and 'acc' is => int(%d)
- strcasecmp of 'acd' and 'Acc' is => int(%d)
- strcasecmp of 'acd' and 'aC' is => int(%d)
- strcasecmp of 'acd' and 'acCc' is => int(%d)
- strcasecmp of 'acd' and 'acd' is => int(0)
- strcasecmp of 'acd' and '?acc' is => int(%d)
- strcasecmp of 'acd' and 'Acc!' is => int(%d)
- strcasecmp of 'acd' and '$!acc' is => int(%d)
- strcasecmp of 'acd' and ';acc' is => int(%d)

Iteration 5
- strcasecmp of '?acc' and 'acc' is => int(-%d)
- strcasecmp of '?acc' and 'Acc' is => int(-%d)
- strcasecmp of '?acc' and 'aC' is => int(-%d)
- strcasecmp of '?acc' and 'acCc' is => int(-%d)
- strcasecmp of '?acc' and 'acd' is => int(-%d)
- strcasecmp of '?acc' and '?acc' is => int(0)
- strcasecmp of '?acc' and 'Acc!' is => int(-%d)
- strcasecmp of '?acc' and '$!acc' is => int(%d)
- strcasecmp of '?acc' and ';acc' is => int(%d)

Iteration 6
- strcasecmp of 'Acc!' and 'acc' is => int(%d)
- strcasecmp of 'Acc!' and 'Acc' is => int(%d)
- strcasecmp of 'Acc!' and 'aC' is => int(%d)
- strcasecmp of 'Acc!' and 'acCc' is => int(-%d)
- strcasecmp of 'Acc!' and 'acd' is => int(-%d)
- strcasecmp of 'Acc!' and '?acc' is => int(%d)
- strcasecmp of 'Acc!' and 'Acc!' is => int(0)
- strcasecmp of 'Acc!' and '$!acc' is => int(%d)
- strcasecmp of 'Acc!' and ';acc' is => int(%d)

Iteration 7
- strcasecmp of '$!acc' and 'acc' is => int(-%d)
- strcasecmp of '$!acc' and 'Acc' is => int(-%d)
- strcasecmp of '$!acc' and 'aC' is => int(-%d)
- strcasecmp of '$!acc' and 'acCc' is => int(-%d)
- strcasecmp of '$!acc' and 'acd' is => int(-%d)
- strcasecmp of '$!acc' and '?acc' is => int(-%d)
- strcasecmp of '$!acc' and 'Acc!' is => int(-%d)
- strcasecmp of '$!acc' and '$!acc' is => int(0)
- strcasecmp of '$!acc' and ';acc' is => int(-%d)

Iteration 8
- strcasecmp of ';acc' and 'acc' is => int(-%d)
- strcasecmp of ';acc' and 'Acc' is => int(-%d)
- strcasecmp of ';acc' and 'aC' is => int(-%d)
- strcasecmp of ';acc' and 'acCc' is => int(-%d)
- strcasecmp of ';acc' and 'acd' is => int(-%d)
- strcasecmp of ';acc' and '?acc' is => int(-%d)
- strcasecmp of ';acc' and 'Acc!' is => int(-%d)
- strcasecmp of ';acc' and '$!acc' is => int(%d)
- strcasecmp of ';acc' and ';acc' is => int(0)

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
    [7] => 1
    [8] => 
    [9] => string
)

Iteration 0
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '0' is => int(%d)
- strcasecmp of '1' and '0' is => int(%d)
- strcasecmp of '1' and '-1' is => int(%d)
- strcasecmp of '1' and '-1' is => int(%d)
- strcasecmp of '1' and '' is => int(%d)
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '' is => int(%d)
- strcasecmp of '1' and 'string' is => int(-%d)

Iteration 1
- strcasecmp of '0' and '1' is => int(-%d)
- strcasecmp of '0' and '0' is => int(0)
- strcasecmp of '0' and '0' is => int(0)
- strcasecmp of '0' and '-1' is => int(%d)
- strcasecmp of '0' and '-1' is => int(%d)
- strcasecmp of '0' and '' is => int(%d)
- strcasecmp of '0' and '1' is => int(-%d)
- strcasecmp of '0' and '1' is => int(-%d)
- strcasecmp of '0' and '' is => int(%d)
- strcasecmp of '0' and 'string' is => int(-%d)

Iteration 2
- strcasecmp of '0' and '1' is => int(-%d)
- strcasecmp of '0' and '0' is => int(0)
- strcasecmp of '0' and '0' is => int(0)
- strcasecmp of '0' and '-1' is => int(%d)
- strcasecmp of '0' and '-1' is => int(%d)
- strcasecmp of '0' and '' is => int(%d)
- strcasecmp of '0' and '1' is => int(-%d)
- strcasecmp of '0' and '1' is => int(-%d)
- strcasecmp of '0' and '' is => int(%d)
- strcasecmp of '0' and 'string' is => int(-%d)

Iteration 3
- strcasecmp of '-1' and '1' is => int(-%d)
- strcasecmp of '-1' and '0' is => int(-%d)
- strcasecmp of '-1' and '0' is => int(-%d)
- strcasecmp of '-1' and '-1' is => int(0)
- strcasecmp of '-1' and '-1' is => int(0)
- strcasecmp of '-1' and '' is => int(%d)
- strcasecmp of '-1' and '1' is => int(-%d)
- strcasecmp of '-1' and '1' is => int(-%d)
- strcasecmp of '-1' and '' is => int(%d)
- strcasecmp of '-1' and 'string' is => int(-%d)

Iteration 4
- strcasecmp of '-1' and '1' is => int(-%d)
- strcasecmp of '-1' and '0' is => int(-%d)
- strcasecmp of '-1' and '0' is => int(-%d)
- strcasecmp of '-1' and '-1' is => int(0)
- strcasecmp of '-1' and '-1' is => int(0)
- strcasecmp of '-1' and '' is => int(%d)
- strcasecmp of '-1' and '1' is => int(-%d)
- strcasecmp of '-1' and '1' is => int(-%d)
- strcasecmp of '-1' and '' is => int(%d)
- strcasecmp of '-1' and 'string' is => int(-%d)

Iteration 5
- strcasecmp of '' and '1' is => int(-%d)
- strcasecmp of '' and '0' is => int(-%d)
- strcasecmp of '' and '0' is => int(-%d)
- strcasecmp of '' and '-1' is => int(-%d)
- strcasecmp of '' and '-1' is => int(-%d)
- strcasecmp of '' and '' is => int(0)
- strcasecmp of '' and '1' is => int(-%d)
- strcasecmp of '' and '1' is => int(-%d)
- strcasecmp of '' and '' is => int(0)
- strcasecmp of '' and 'string' is => int(-%d)

Iteration 6
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '0' is => int(%d)
- strcasecmp of '1' and '0' is => int(%d)
- strcasecmp of '1' and '-1' is => int(%d)
- strcasecmp of '1' and '-1' is => int(%d)
- strcasecmp of '1' and '' is => int(%d)
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '' is => int(%d)
- strcasecmp of '1' and 'string' is => int(-%d)

Iteration 7
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '0' is => int(%d)
- strcasecmp of '1' and '0' is => int(%d)
- strcasecmp of '1' and '-1' is => int(%d)
- strcasecmp of '1' and '-1' is => int(%d)
- strcasecmp of '1' and '' is => int(%d)
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '1' is => int(0)
- strcasecmp of '1' and '' is => int(%d)
- strcasecmp of '1' and 'string' is => int(-%d)

Iteration 8
- strcasecmp of '' and '1' is => int(-%d)
- strcasecmp of '' and '0' is => int(-%d)
- strcasecmp of '' and '0' is => int(-%d)
- strcasecmp of '' and '-1' is => int(-%d)
- strcasecmp of '' and '-1' is => int(-%d)
- strcasecmp of '' and '' is => int(0)
- strcasecmp of '' and '1' is => int(-%d)
- strcasecmp of '' and '1' is => int(-%d)
- strcasecmp of '' and '' is => int(0)
- strcasecmp of '' and 'string' is => int(-%d)

Iteration 9
- strcasecmp of 'string' and '1' is => int(%d)
- strcasecmp of 'string' and '0' is => int(%d)
- strcasecmp of 'string' and '0' is => int(%d)
- strcasecmp of 'string' and '-1' is => int(%d)
- strcasecmp of 'string' and '-1' is => int(%d)
- strcasecmp of 'string' and '' is => int(%d)
- strcasecmp of 'string' and '1' is => int(%d)
- strcasecmp of 'string' and '1' is => int(%d)
- strcasecmp of 'string' and '' is => int(%d)
- strcasecmp of 'string' and 'string' is => int(0)

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
- strcasecmp of '10.5' and '10.5' is => int(0)
- strcasecmp of '10.5' and '1.5' is => int(%d)
- strcasecmp of '10.5' and '9.5' is => int(-%d)
- strcasecmp of '10.5' and '11.5' is => int(-%d)
- strcasecmp of '10.5' and '100.5' is => int(-%d)
- strcasecmp of '10.5' and '105' is => int(-%d)
- strcasecmp of '10.5' and '-10.5' is => int(%d)
- strcasecmp of '10.5' and '10' is => int(%d)
- strcasecmp of '10.5' and '0.5' is => int(%d)

Iteration 1
- strcasecmp of '1.5' and '10.5' is => int(-%d)
- strcasecmp of '1.5' and '1.5' is => int(0)
- strcasecmp of '1.5' and '9.5' is => int(-%d)
- strcasecmp of '1.5' and '11.5' is => int(-%d)
- strcasecmp of '1.5' and '100.5' is => int(-%d)
- strcasecmp of '1.5' and '105' is => int(-%d)
- strcasecmp of '1.5' and '-10.5' is => int(%d)
- strcasecmp of '1.5' and '10' is => int(-%d)
- strcasecmp of '1.5' and '0.5' is => int(%d)

Iteration 2
- strcasecmp of '9.5' and '10.5' is => int(%d)
- strcasecmp of '9.5' and '1.5' is => int(%d)
- strcasecmp of '9.5' and '9.5' is => int(0)
- strcasecmp of '9.5' and '11.5' is => int(%d)
- strcasecmp of '9.5' and '100.5' is => int(%d)
- strcasecmp of '9.5' and '105' is => int(%d)
- strcasecmp of '9.5' and '-10.5' is => int(%d)
- strcasecmp of '9.5' and '10' is => int(%d)
- strcasecmp of '9.5' and '0.5' is => int(%d)

Iteration 3
- strcasecmp of '11.5' and '10.5' is => int(%d)
- strcasecmp of '11.5' and '1.5' is => int(%d)
- strcasecmp of '11.5' and '9.5' is => int(-%d)
- strcasecmp of '11.5' and '11.5' is => int(0)
- strcasecmp of '11.5' and '100.5' is => int(%d)
- strcasecmp of '11.5' and '105' is => int(%d)
- strcasecmp of '11.5' and '-10.5' is => int(%d)
- strcasecmp of '11.5' and '10' is => int(%d)
- strcasecmp of '11.5' and '0.5' is => int(%d)

Iteration 4
- strcasecmp of '100.5' and '10.5' is => int(%d)
- strcasecmp of '100.5' and '1.5' is => int(%d)
- strcasecmp of '100.5' and '9.5' is => int(-%d)
- strcasecmp of '100.5' and '11.5' is => int(-%d)
- strcasecmp of '100.5' and '100.5' is => int(0)
- strcasecmp of '100.5' and '105' is => int(-%d)
- strcasecmp of '100.5' and '-10.5' is => int(%d)
- strcasecmp of '100.5' and '10' is => int(%d)
- strcasecmp of '100.5' and '0.5' is => int(%d)

Iteration 5
- strcasecmp of '105' and '10.5' is => int(%d)
- strcasecmp of '105' and '1.5' is => int(%d)
- strcasecmp of '105' and '9.5' is => int(-%d)
- strcasecmp of '105' and '11.5' is => int(-%d)
- strcasecmp of '105' and '100.5' is => int(%d)
- strcasecmp of '105' and '105' is => int(0)
- strcasecmp of '105' and '-10.5' is => int(%d)
- strcasecmp of '105' and '10' is => int(%d)
- strcasecmp of '105' and '0.5' is => int(%d)

Iteration 6
- strcasecmp of '-10.5' and '10.5' is => int(-%d)
- strcasecmp of '-10.5' and '1.5' is => int(-%d)
- strcasecmp of '-10.5' and '9.5' is => int(-%d)
- strcasecmp of '-10.5' and '11.5' is => int(-%d)
- strcasecmp of '-10.5' and '100.5' is => int(-%d)
- strcasecmp of '-10.5' and '105' is => int(-%d)
- strcasecmp of '-10.5' and '-10.5' is => int(0)
- strcasecmp of '-10.5' and '10' is => int(-%d)
- strcasecmp of '-10.5' and '0.5' is => int(-%d)

Iteration 7
- strcasecmp of '10' and '10.5' is => int(-%d)
- strcasecmp of '10' and '1.5' is => int(%d)
- strcasecmp of '10' and '9.5' is => int(-%d)
- strcasecmp of '10' and '11.5' is => int(-%d)
- strcasecmp of '10' and '100.5' is => int(-%d)
- strcasecmp of '10' and '105' is => int(-%d)
- strcasecmp of '10' and '-10.5' is => int(%d)
- strcasecmp of '10' and '10' is => int(0)
- strcasecmp of '10' and '0.5' is => int(%d)

Iteration 8
- strcasecmp of '0.5' and '10.5' is => int(-%d)
- strcasecmp of '0.5' and '1.5' is => int(-%d)
- strcasecmp of '0.5' and '9.5' is => int(-%d)
- strcasecmp of '0.5' and '11.5' is => int(-%d)
- strcasecmp of '0.5' and '100.5' is => int(-%d)
- strcasecmp of '0.5' and '105' is => int(-%d)
- strcasecmp of '0.5' and '-10.5' is => int(%d)
- strcasecmp of '0.5' and '10' is => int(-%d)
- strcasecmp of '0.5' and '0.5' is => int(0)

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
