--TEST--
Test preg_match_all() function : basic functionality
--FILE--
<?php
$string = 'Hello, world! This is a test. This is another test. \[4]. 34534 string.';

var_dump(preg_match_all('/[0-35-9]/', $string, $match1, PREG_OFFSET_CAPTURE|PREG_PATTERN_ORDER, -10));               	//finds any digit that's not 4 10 digits from the end(1 match)
var_dump($match1);

var_dump(preg_match_all('/[tT]his is a(.*?)\./', $string, $match2, PREG_SET_ORDER));    						//finds "This is a test." and "This is another test." (non-greedy) (2 matches)
var_dump($match2);

var_dump(preg_match_all('@\. \\\(.*).@', $string, $match3, PREG_PATTERN_ORDER));            				//finds ".\ [...]" and everything else to the end of the string. (greedy) (1 match)
var_dump($match3);

var_dump(preg_match_all('/\d{2}$/', $string, $match4));										//tries to find 2 digits at the end of a string (0 matches)
var_dump($match4);

var_dump(preg_match_all('/(This is a ){2}(.*)\stest/', $string, $match5));							//tries to find "This is aThis is a [...] test" (0 matches)
var_dump($match5);
?>
--EXPECT--
int(1)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(2) {
      [0]=>
      string(1) "3"
      [1]=>
      int(61)
    }
  }
}
int(2)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(15) "This is a test."
    [1]=>
    string(5) " test"
  }
  [1]=>
  array(2) {
    [0]=>
    string(21) "This is another test."
    [1]=>
    string(11) "nother test"
  }
}
int(1)
array(2) {
  [0]=>
  array(1) {
    [0]=>
    string(21) ". \[4]. 34534 string."
  }
  [1]=>
  array(1) {
    [0]=>
    string(17) "[4]. 34534 string"
  }
}
int(0)
array(1) {
  [0]=>
  array(0) {
  }
}
int(0)
array(3) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
  [2]=>
  array(0) {
  }
}
