--TEST--
Test array_key_exists() function : usage variations - array keys are different data types
--FILE--
<?php
/*
 * Pass an array where the keys are different data types as the $search argument
 * then pass many different data types as $key argument to test where array_key_exist()
 * returns true.
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

// heredoc string
$heredoc = <<<EOT
string
EOT;

// different data types to be iterated over
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // boolean data
/*4*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),

       // empty data
/*5*/ 'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*6*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),
);

// loop through each element of $inputs to check the behavior of array_key_exists()
$iterator = 1;
foreach($inputs as $type => $input) {
    echo "\n-- $type data --\n";

    //iterate over again to get all different key values
    foreach ($inputs as $new_type => $new_input) {
        echo "-- \$key arguments are $new_type data:\n";
        foreach ($new_input as $key => $search) {
            var_dump(array_key_exists($key, $input));
        }
    }
    $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_key_exists() : usage variations ***

-- int data --
-- $key arguments are int data:
bool(true)
bool(true)
bool(true)
bool(true)
-- $key arguments are bool lowercase data:
bool(true)
bool(true)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)

-- bool lowercase data --
-- $key arguments are int data:
bool(true)
bool(true)
bool(false)
bool(false)
-- $key arguments are bool lowercase data:
bool(true)
bool(true)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)

-- empty single quotes data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are empty single quotes data:
bool(true)
-- $key arguments are string data:
bool(false)
bool(false)
bool(false)

-- string data --
-- $key arguments are int data:
bool(false)
bool(false)
bool(false)
bool(false)
-- $key arguments are bool lowercase data:
bool(false)
bool(false)
-- $key arguments are empty single quotes data:
bool(false)
-- $key arguments are string data:
bool(true)
bool(true)
bool(true)
Done
