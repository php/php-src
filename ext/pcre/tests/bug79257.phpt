--TEST--
Bug #79257: Duplicate named capture return last alternative even if unmatched
--FILE--
<?php

preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))(geez)/', 'foogeez', $matches);
var_dump($matches);

preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))(geez)/', 'foogeez', $matches, PREG_OFFSET_CAPTURE);
var_dump($matches);

preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))(geez)/', 'foogeez', $matches, PREG_UNMATCHED_AS_NULL);
var_dump($matches);

preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))(geez)/', 'foogeez', $matches, PREG_UNMATCHED_AS_NULL | PREG_OFFSET_CAPTURE);
var_dump($matches);

?>
--EXPECT--
array(5) {
  [0]=>
  string(7) "foogeez"
  ["g"]=>
  string(3) "foo"
  [1]=>
  string(3) "foo"
  [2]=>
  string(0) ""
  [3]=>
  string(4) "geez"
}
array(5) {
  [0]=>
  array(2) {
    [0]=>
    string(7) "foogeez"
    [1]=>
    int(0)
  }
  ["g"]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    int(0)
  }
  [2]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(-1)
  }
  [3]=>
  array(2) {
    [0]=>
    string(4) "geez"
    [1]=>
    int(3)
  }
}
array(5) {
  [0]=>
  string(7) "foogeez"
  ["g"]=>
  string(3) "foo"
  [1]=>
  string(3) "foo"
  [2]=>
  NULL
  [3]=>
  string(4) "geez"
}
array(5) {
  [0]=>
  array(2) {
    [0]=>
    string(7) "foogeez"
    [1]=>
    int(0)
  }
  ["g"]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    int(0)
  }
  [2]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    int(-1)
  }
  [3]=>
  array(2) {
    [0]=>
    string(4) "geez"
    [1]=>
    int(3)
  }
}
