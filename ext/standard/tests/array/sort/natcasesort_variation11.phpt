--TEST--
Test natcasesort() function : usage variations - Different array keys
--FILE--
<?php
/*
 * Pass arrays where the keys are different data types to test behaviour of natcasesort()
 */

echo "*** Testing natcasesort() : usage variations ***\n";

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// arrays with keys as different data types to be passed as $array_arg
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // boolean data
/*6*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),

/*9*/  'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*10*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),

       // duplicate values
/*13*/ 'duplicate' => array(
       'foo' => 'bar',
       'baz' => 'bar',
       'hello' => 'world'
       ),

);

// loop through each element of $inputs to check the behavior of natcasesort()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump( natcasesort($input) );
    var_dump($input);
    $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing natcasesort() : usage variations ***

-- Iteration 1 --
bool(true)
array(4) {
  [-2345]=>
  string(8) "negative"
  [1]=>
  string(3) "one"
  [12345]=>
  string(8) "positive"
  [0]=>
  string(4) "zero"
}

-- Iteration 2 --
bool(true)
array(2) {
  [0]=>
  string(6) "lowerf"
  [1]=>
  string(6) "lowert"
}

-- Iteration 3 --
bool(true)
array(1) {
  [""]=>
  string(6) "emptys"
}

-- Iteration 4 --
bool(true)
array(3) {
  ["stringd"]=>
  string(7) "stringd"
  ["hello world"]=>
  string(7) "stringh"
  ["strings"]=>
  string(7) "strings"
}

-- Iteration 5 --
bool(true)
array(3) {
  ["foo"]=>
  string(3) "bar"
  ["baz"]=>
  string(3) "bar"
  ["hello"]=>
  string(5) "world"
}
Done
