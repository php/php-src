--TEST--
Test mb_split() function : usage variations  - different parameter types for pattern
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_split') or die("skip mb_split() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : proto array mb_split(string pattern, string string [, int limit])
 * Description: split multibyte string into array by regular expression 
 * Source code: ext/mbstring/php_mbregex.c
 * Alias to functions: 
 */

echo "*** Testing mb_split() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$string = 'a b c d e f g';
$limit = 10;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "UTF-8";
  }
}

// heredoc string
$heredoc = <<<EOT
UTF-8
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $encoding argument
$inputs = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,
       
       // empty data
/*16*/ "",
       '',

       // string data
/*18*/ "UTF-8",
       'UTF-8',
       $heredoc,
       
       // object data
/*21*/ new classA(),

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element of the array for pattern

$iterator = 1;
foreach($inputs as $input) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( mb_split($input, $string, $limit) );
      $iterator++;
};
fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing mb_split() : usage variations ***

-- Iteration 1 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 2 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 3 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 4 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 5 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 6 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 7 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 8 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 9 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 10 --

Warning: mb_split(): Empty regular expression in %s on line %d
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 11 --

Warning: mb_split(): Empty regular expression in %s on line %d
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 12 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 13 --

Warning: mb_split(): Empty regular expression in %s on line %d
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 14 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 15 --

Warning: mb_split(): Empty regular expression in %s on line %d
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 16 --

Warning: mb_split(): Empty regular expression in %s on line %d
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 17 --

Warning: mb_split(): Empty regular expression in %s on line %d
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 18 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 19 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 20 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 21 --
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 22 --

Warning: mb_split(): Empty regular expression in %s on line %d
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 23 --

Warning: mb_split(): Empty regular expression in %s on line %d
array(1) {
  [0]=>
  string(13) "a b c d e f g"
}

-- Iteration 24 --

Warning: mb_split() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
Done