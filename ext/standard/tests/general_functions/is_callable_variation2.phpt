--TEST--
Test is_callable() function : usage variations - on invalid function names
--FILE--
<?php
function check_iscallable( $functions ) {
  $counter = 1;
  foreach($functions as $func) {
    echo "-- Iteration  $counter --\n";
    var_dump( is_callable($func) );  //given only $var argument
    var_dump( is_callable($func, TRUE) );  //given $var and $syntax argument
    var_dump( is_callable($func, TRUE, $callable_name) );
    echo $callable_name, "\n";
    var_dump( is_callable($func, FALSE) );  //given $var and $syntax argument
    var_dump( is_callable($func, FALSE, $callable_name) );
    echo $callable_name, "\n";
    $counter++;
  }
}

echo "\n*** Testing is_callable() on invalid function names ***\n";
/* check on unset variables */
$unset_var = 10;
unset ($unset_var);

/* opening file resource type */
$file_handle = fopen (__FILE__, "r");

$variants = array (
  NULL,  // NULL as argument
  0,  // zero as argument
  1234567890,  // positive value
  -100123456782,  // negative value
  -2.000000,  // negative float value
  .567,  // positive float value
  FALSE,  // boolean value
  array(1, 2, 3),  // array
  @$unset_var,
  @$undef_var,  //undefined variable
  $file_handle
);

/* use check_iscallable() to check whether given variable is valid function name
 *  expected: false
 */
check_iscallable($variants);

/* closing resources used */
fclose($file_handle);

?>
--EXPECTF--
*** Testing is_callable() on invalid function names ***
-- Iteration  1 --
bool(false)
bool(false)
bool(false)

bool(false)
bool(false)

-- Iteration  2 --
bool(false)
bool(false)
bool(false)
0
bool(false)
bool(false)
0
-- Iteration  3 --
bool(false)
bool(false)
bool(false)
1234567890
bool(false)
bool(false)
1234567890
-- Iteration  4 --
bool(false)
bool(false)
bool(false)
-100123456782
bool(false)
bool(false)
-100123456782
-- Iteration  5 --
bool(false)
bool(false)
bool(false)
-2
bool(false)
bool(false)
-2
-- Iteration  6 --
bool(false)
bool(false)
bool(false)
0.567
bool(false)
bool(false)
0.567
-- Iteration  7 --
bool(false)
bool(false)
bool(false)

bool(false)
bool(false)

-- Iteration  8 --
bool(false)
bool(false)
bool(false)
Array
bool(false)
bool(false)
Array
-- Iteration  9 --
bool(false)
bool(false)
bool(false)

bool(false)
bool(false)

-- Iteration  10 --
bool(false)
bool(false)
bool(false)

bool(false)
bool(false)

-- Iteration  11 --
bool(false)
bool(false)
bool(false)
Resource id #%d
bool(false)
bool(false)
Resource id #%d
