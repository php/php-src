--TEST--
Test str_repeat() function
--INI--
precision=14
--FILE--
<?php
echo "*** Testing str_repeat() with possible strings ***";
$variations = array(
  'a',
  'foo',
  'barbazbax',
  "\x00",
  '\0',
  TRUE,
  4,
  1.23,
  "",
  " "
);

/* variations in string and multiplier as an int */
foreach($variations as $input) {
  echo "\n--- str_repeat() of '$input' ---\n" ;
  for($n=0; $n<4; $n++) {
    echo "-- after repeating $n times is => ";
    echo str_repeat($input, $n)."\n";
  }
}

echo "\n\n*** Testing error conditions ***\n";
try {
    str_repeat($input[0], -1); // Invalid arg for multiplier
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>

--EXPECTF--
*** Testing str_repeat() with possible strings ***
--- str_repeat() of 'a' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => a
-- after repeating 2 times is => aa
-- after repeating 3 times is => aaa

--- str_repeat() of 'foo' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => foo
-- after repeating 2 times is => foofoo
-- after repeating 3 times is => foofoofoo

--- str_repeat() of 'barbazbax' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => barbazbax
-- after repeating 2 times is => barbazbaxbarbazbax
-- after repeating 3 times is => barbazbaxbarbazbaxbarbazbax

--- str_repeat() of '%0' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => %0
-- after repeating 2 times is => %0%0
-- after repeating 3 times is => %0%0%0

--- str_repeat() of '\0' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => \0
-- after repeating 2 times is => \0\0
-- after repeating 3 times is => \0\0\0

--- str_repeat() of '1' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => 1
-- after repeating 2 times is => 11
-- after repeating 3 times is => 111

--- str_repeat() of '4' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => 4
-- after repeating 2 times is => 44
-- after repeating 3 times is => 444

--- str_repeat() of '1.23' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => 1.23
-- after repeating 2 times is => 1.231.23
-- after repeating 3 times is => 1.231.231.23

--- str_repeat() of '' ---
-- after repeating 0 times is => 
-- after repeating 1 times is => 
-- after repeating 2 times is => 
-- after repeating 3 times is => 

--- str_repeat() of ' ' ---
-- after repeating 0 times is => 
-- after repeating 1 times is =>  
-- after repeating 2 times is =>   
-- after repeating 3 times is =>    


*** Testing error conditions ***
str_repeat(): Argument #2 ($times) must be greater than or equal to 0
