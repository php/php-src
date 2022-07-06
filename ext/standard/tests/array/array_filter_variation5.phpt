--TEST--
Test array_filter() function : usage variations - 'input' argument with different false entries
--FILE--
<?php
/*
* With default callback function argument, array_filter() removes elements which are interpreted as false
* Here Testing all the false array element possibilities
*/

function always_true($input)
{
  return true;
}

// callback function always_false
function always_false($input)
{
  return false;
}

echo "*** Testing array_filter() : usage variations - different false elements in 'input' ***\n";

// unset variable
$unset_var = 10;
unset($unset_var);

// empty heredoc string
$empty_heredoc =<<<EOT
EOT;

// input array with different false elements
$input = array(
  false,
  False,
  '',
  "",
  0,
  0.0,
  null,
  NULL,
  "0",
  '0',
  array(),
  !1,
  1==2,
  $empty_heredoc,
  @$unset_var,
  @$undefined_var,
);

// With default callback function
var_dump( array_filter($input) );

// With callback function which returns always true
var_dump( array_filter($input, 'always_true') );

// With callback function which returns always false
var_dump( array_filter($input, 'always_false') );

echo "Done"
?>
--EXPECT--
*** Testing array_filter() : usage variations - different false elements in 'input' ***
array(0) {
}
array(16) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  int(0)
  [5]=>
  float(0)
  [6]=>
  NULL
  [7]=>
  NULL
  [8]=>
  string(1) "0"
  [9]=>
  string(1) "0"
  [10]=>
  array(0) {
  }
  [11]=>
  bool(false)
  [12]=>
  bool(false)
  [13]=>
  string(0) ""
  [14]=>
  NULL
  [15]=>
  NULL
}
array(0) {
}
Done
