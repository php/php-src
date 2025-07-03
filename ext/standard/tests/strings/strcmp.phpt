--TEST--
strcmp() function
--INI--
precision=12
--FILE--
<?php
/* Compares two strings in case-sensitive manner */

echo "#### Basic and Possible operations ####";
/* creating an array of strings to be compared */
$arrays = [
    ["a", 'A'],
    ["acc", "Acc", 'aC', "acCc", 'acd', "?acc", 'Acc!', "$!acc", ";acc"],
    ["1", "0", "-1", "", "string"],
];

/* loop through to go each and every element in an array
    and comparing the elements with one and other */
foreach($arrays as $str1_arr){
    echo "\n*** comparing the strings in an\n";
    var_dump($str1_arr);
    for ($i=0; $i<count($str1_arr); $i++){
        echo "\nIteration $i\n";
        foreach ($str1_arr as $str2){
            echo "- strcmp of '$str1_arr[$i]' and '$str2' is => ";
            var_dump(strcmp($str1_arr[$i], $str2));
        }
    }
}

echo "\n--- Comparing long float values ---\n";
/* Here two different outputs, which depends on the rounding value
   before converting to string. Here Precision = 12  */
var_dump(strcmp(10.55555555555555555555555555, 10.5555555556));   // int(0)
var_dump(strcmp(10.55555555555555555555555555, 10.555555556));    // int(-1)
var_dump(strcmp(10.55555555595555555555555555, 10.555555556));    // int(0)

echo "Done\n";
?>
--EXPECTF--
#### Basic and Possible operations ####
*** comparing the strings in an
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "A"
}

Iteration 0
- strcmp of 'a' and 'a' is => int(0)
- strcmp of 'a' and 'A' is => int(%d)

Iteration 1
- strcmp of 'A' and 'a' is => int(-%d)
- strcmp of 'A' and 'A' is => int(0)

*** comparing the strings in an
array(9) {
  [0]=>
  string(3) "acc"
  [1]=>
  string(3) "Acc"
  [2]=>
  string(2) "aC"
  [3]=>
  string(4) "acCc"
  [4]=>
  string(3) "acd"
  [5]=>
  string(4) "?acc"
  [6]=>
  string(4) "Acc!"
  [7]=>
  string(5) "$!acc"
  [8]=>
  string(4) ";acc"
}

Iteration 0
- strcmp of 'acc' and 'acc' is => int(0)
- strcmp of 'acc' and 'Acc' is => int(%d)
- strcmp of 'acc' and 'aC' is => int(%d)
- strcmp of 'acc' and 'acCc' is => int(%d)
- strcmp of 'acc' and 'acd' is => int(-%d)
- strcmp of 'acc' and '?acc' is => int(%d)
- strcmp of 'acc' and 'Acc!' is => int(%d)
- strcmp of 'acc' and '$!acc' is => int(%d)
- strcmp of 'acc' and ';acc' is => int(%d)

Iteration 1
- strcmp of 'Acc' and 'acc' is => int(-%d)
- strcmp of 'Acc' and 'Acc' is => int(0)
- strcmp of 'Acc' and 'aC' is => int(-%d)
- strcmp of 'Acc' and 'acCc' is => int(-%d)
- strcmp of 'Acc' and 'acd' is => int(-%d)
- strcmp of 'Acc' and '?acc' is => int(%d)
- strcmp of 'Acc' and 'Acc!' is => int(-%d)
- strcmp of 'Acc' and '$!acc' is => int(%d)
- strcmp of 'Acc' and ';acc' is => int(%d)

Iteration 2
- strcmp of 'aC' and 'acc' is => int(-%d)
- strcmp of 'aC' and 'Acc' is => int(%d)
- strcmp of 'aC' and 'aC' is => int(0)
- strcmp of 'aC' and 'acCc' is => int(-%d)
- strcmp of 'aC' and 'acd' is => int(-%d)
- strcmp of 'aC' and '?acc' is => int(%d)
- strcmp of 'aC' and 'Acc!' is => int(%d)
- strcmp of 'aC' and '$!acc' is => int(%d)
- strcmp of 'aC' and ';acc' is => int(%d)

Iteration 3
- strcmp of 'acCc' and 'acc' is => int(-%d)
- strcmp of 'acCc' and 'Acc' is => int(%d)
- strcmp of 'acCc' and 'aC' is => int(%d)
- strcmp of 'acCc' and 'acCc' is => int(0)
- strcmp of 'acCc' and 'acd' is => int(-%d)
- strcmp of 'acCc' and '?acc' is => int(%d)
- strcmp of 'acCc' and 'Acc!' is => int(%d)
- strcmp of 'acCc' and '$!acc' is => int(%d)
- strcmp of 'acCc' and ';acc' is => int(%d)

Iteration 4
- strcmp of 'acd' and 'acc' is => int(%d)
- strcmp of 'acd' and 'Acc' is => int(%d)
- strcmp of 'acd' and 'aC' is => int(%d)
- strcmp of 'acd' and 'acCc' is => int(%d)
- strcmp of 'acd' and 'acd' is => int(0)
- strcmp of 'acd' and '?acc' is => int(%d)
- strcmp of 'acd' and 'Acc!' is => int(%d)
- strcmp of 'acd' and '$!acc' is => int(%d)
- strcmp of 'acd' and ';acc' is => int(%d)

Iteration 5
- strcmp of '?acc' and 'acc' is => int(-%d)
- strcmp of '?acc' and 'Acc' is => int(-%d)
- strcmp of '?acc' and 'aC' is => int(-%d)
- strcmp of '?acc' and 'acCc' is => int(-%d)
- strcmp of '?acc' and 'acd' is => int(-%d)
- strcmp of '?acc' and '?acc' is => int(0)
- strcmp of '?acc' and 'Acc!' is => int(-%d)
- strcmp of '?acc' and '$!acc' is => int(%d)
- strcmp of '?acc' and ';acc' is => int(%d)

Iteration 6
- strcmp of 'Acc!' and 'acc' is => int(-%d)
- strcmp of 'Acc!' and 'Acc' is => int(%d)
- strcmp of 'Acc!' and 'aC' is => int(-%d)
- strcmp of 'Acc!' and 'acCc' is => int(-%d)
- strcmp of 'Acc!' and 'acd' is => int(-%d)
- strcmp of 'Acc!' and '?acc' is => int(%d)
- strcmp of 'Acc!' and 'Acc!' is => int(0)
- strcmp of 'Acc!' and '$!acc' is => int(%d)
- strcmp of 'Acc!' and ';acc' is => int(%d)

Iteration 7
- strcmp of '$!acc' and 'acc' is => int(-%d)
- strcmp of '$!acc' and 'Acc' is => int(-%d)
- strcmp of '$!acc' and 'aC' is => int(-%d)
- strcmp of '$!acc' and 'acCc' is => int(-%d)
- strcmp of '$!acc' and 'acd' is => int(-%d)
- strcmp of '$!acc' and '?acc' is => int(-%d)
- strcmp of '$!acc' and 'Acc!' is => int(-%d)
- strcmp of '$!acc' and '$!acc' is => int(0)
- strcmp of '$!acc' and ';acc' is => int(-%d)

Iteration 8
- strcmp of ';acc' and 'acc' is => int(-%d)
- strcmp of ';acc' and 'Acc' is => int(-%d)
- strcmp of ';acc' and 'aC' is => int(-%d)
- strcmp of ';acc' and 'acCc' is => int(-%d)
- strcmp of ';acc' and 'acd' is => int(-%d)
- strcmp of ';acc' and '?acc' is => int(-%d)
- strcmp of ';acc' and 'Acc!' is => int(-%d)
- strcmp of ';acc' and '$!acc' is => int(%d)
- strcmp of ';acc' and ';acc' is => int(0)

*** comparing the strings in an
array(5) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "0"
  [2]=>
  string(2) "-1"
  [3]=>
  string(0) ""
  [4]=>
  string(6) "string"
}

Iteration 0
- strcmp of '1' and '1' is => int(0)
- strcmp of '1' and '0' is => int(%d)
- strcmp of '1' and '-1' is => int(%d)
- strcmp of '1' and '' is => int(%d)
- strcmp of '1' and 'string' is => int(-%d)

Iteration 1
- strcmp of '0' and '1' is => int(-%d)
- strcmp of '0' and '0' is => int(0)
- strcmp of '0' and '-1' is => int(%d)
- strcmp of '0' and '' is => int(%d)
- strcmp of '0' and 'string' is => int(-%d)

Iteration 2
- strcmp of '-1' and '1' is => int(-%d)
- strcmp of '-1' and '0' is => int(-%d)
- strcmp of '-1' and '-1' is => int(0)
- strcmp of '-1' and '' is => int(%d)
- strcmp of '-1' and 'string' is => int(-%d)

Iteration 3
- strcmp of '' and '1' is => int(-%d)
- strcmp of '' and '0' is => int(-%d)
- strcmp of '' and '-1' is => int(-%d)
- strcmp of '' and '' is => int(0)
- strcmp of '' and 'string' is => int(-%d)

Iteration 4
- strcmp of 'string' and '1' is => int(%d)
- strcmp of 'string' and '0' is => int(%d)
- strcmp of 'string' and '-1' is => int(%d)
- strcmp of 'string' and '' is => int(%d)
- strcmp of 'string' and 'string' is => int(0)

--- Comparing long float values ---
int(0)
int(-1)
int(0)
Done
