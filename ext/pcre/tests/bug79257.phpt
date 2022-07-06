--TEST--
Bug #79257: Duplicate named groups (?J) prefer last alternative even if not matched
--FILE--
<?php

preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))/', 'foo', $matches);
var_dump($matches);
preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))/', 'foo', $matches,
    PREG_UNMATCHED_AS_NULL);
var_dump($matches);
preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))/', 'foo', $matches,
    PREG_OFFSET_CAPTURE);
var_dump($matches);
preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))/', 'foo', $matches,
    PREG_UNMATCHED_AS_NULL|PREG_OFFSET_CAPTURE);
var_dump($matches);

preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))(?<h>baz)/', 'foobaz', $matches);
var_dump($matches);
preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))(?<h>baz)/', 'foobaz', $matches,
    PREG_UNMATCHED_AS_NULL);
var_dump($matches);
preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))(?<h>baz)/', 'foobaz', $matches,
    PREG_OFFSET_CAPTURE);
var_dump($matches);
preg_match('/(?J)(?:(?<g>foo)|(?<g>bar))(?<h>baz)/', 'foobaz', $matches,
    PREG_UNMATCHED_AS_NULL|PREG_OFFSET_CAPTURE);
var_dump($matches);

?>
--EXPECT--
array(3) {
  [0]=>
  string(3) "foo"
  ["g"]=>
  string(3) "foo"
  [1]=>
  string(3) "foo"
}
array(4) {
  [0]=>
  string(3) "foo"
  ["g"]=>
  string(3) "foo"
  [1]=>
  string(3) "foo"
  [2]=>
  NULL
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "foo"
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
}
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "foo"
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
}
array(6) {
  [0]=>
  string(6) "foobaz"
  ["g"]=>
  string(3) "foo"
  [1]=>
  string(3) "foo"
  [2]=>
  string(0) ""
  ["h"]=>
  string(3) "baz"
  [3]=>
  string(3) "baz"
}
array(6) {
  [0]=>
  string(6) "foobaz"
  ["g"]=>
  string(3) "foo"
  [1]=>
  string(3) "foo"
  [2]=>
  NULL
  ["h"]=>
  string(3) "baz"
  [3]=>
  string(3) "baz"
}
array(6) {
  [0]=>
  array(2) {
    [0]=>
    string(6) "foobaz"
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
  ["h"]=>
  array(2) {
    [0]=>
    string(3) "baz"
    [1]=>
    int(3)
  }
  [3]=>
  array(2) {
    [0]=>
    string(3) "baz"
    [1]=>
    int(3)
  }
}
array(6) {
  [0]=>
  array(2) {
    [0]=>
    string(6) "foobaz"
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
  ["h"]=>
  array(2) {
    [0]=>
    string(3) "baz"
    [1]=>
    int(3)
  }
  [3]=>
  array(2) {
    [0]=>
    string(3) "baz"
    [1]=>
    int(3)
  }
}
