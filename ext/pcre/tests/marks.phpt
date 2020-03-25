--TEST--
Test support for PCRE marks
--FILE--
<?php

$regex = <<<'REGEX'
/
    _ (a) (*MARK:A_MARK) _
  | _ (b) _
  | _ (c) (*MARK:C_MARK) _
  | _ (d) _
/x
REGEX;

var_dump(preg_match($regex, '_c_', $matches));
var_dump($matches);

var_dump(preg_match_all($regex, '_a__b__c__d_', $matches, PREG_PATTERN_ORDER));
var_dump($matches);

var_dump(preg_match_all($regex, '_a__b__c__d_', $matches, PREG_SET_ORDER));
var_dump($matches);

var_dump(preg_replace_callback($regex, function($matches) {
    var_dump($matches);
    return $matches[0];
}, '_a__b__c__d_'));

?>
--EXPECT--
int(1)
array(5) {
  [0]=>
  string(3) "_c_"
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(1) "c"
  ["MARK"]=>
  string(6) "C_MARK"
}
int(4)
array(6) {
  [0]=>
  array(4) {
    [0]=>
    string(3) "_a_"
    [1]=>
    string(3) "_b_"
    [2]=>
    string(3) "_c_"
    [3]=>
    string(3) "_d_"
  }
  [1]=>
  array(4) {
    [0]=>
    string(1) "a"
    [1]=>
    string(0) ""
    [2]=>
    string(0) ""
    [3]=>
    string(0) ""
  }
  [2]=>
  array(4) {
    [0]=>
    string(0) ""
    [1]=>
    string(1) "b"
    [2]=>
    string(0) ""
    [3]=>
    string(0) ""
  }
  [3]=>
  array(4) {
    [0]=>
    string(0) ""
    [1]=>
    string(0) ""
    [2]=>
    string(1) "c"
    [3]=>
    string(0) ""
  }
  [4]=>
  array(4) {
    [0]=>
    string(0) ""
    [1]=>
    string(0) ""
    [2]=>
    string(0) ""
    [3]=>
    string(1) "d"
  }
  ["MARK"]=>
  array(2) {
    [0]=>
    string(6) "A_MARK"
    [2]=>
    string(6) "C_MARK"
  }
}
int(4)
array(4) {
  [0]=>
  array(3) {
    [0]=>
    string(3) "_a_"
    [1]=>
    string(1) "a"
    ["MARK"]=>
    string(6) "A_MARK"
  }
  [1]=>
  array(3) {
    [0]=>
    string(3) "_b_"
    [1]=>
    string(0) ""
    [2]=>
    string(1) "b"
  }
  [2]=>
  array(5) {
    [0]=>
    string(3) "_c_"
    [1]=>
    string(0) ""
    [2]=>
    string(0) ""
    [3]=>
    string(1) "c"
    ["MARK"]=>
    string(6) "C_MARK"
  }
  [3]=>
  array(5) {
    [0]=>
    string(3) "_d_"
    [1]=>
    string(0) ""
    [2]=>
    string(0) ""
    [3]=>
    string(0) ""
    [4]=>
    string(1) "d"
  }
}
array(3) {
  [0]=>
  string(3) "_a_"
  [1]=>
  string(1) "a"
  ["MARK"]=>
  string(6) "A_MARK"
}
array(3) {
  [0]=>
  string(3) "_b_"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "b"
}
array(5) {
  [0]=>
  string(3) "_c_"
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(1) "c"
  ["MARK"]=>
  string(6) "C_MARK"
}
array(5) {
  [0]=>
  string(3) "_d_"
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(1) "d"
}
string(12) "_a__b__c__d_"
