--TEST--
Basic callout functionality
--FILE--
<?php

preg_callout_function(function($info) {
    var_dump($info);
    return 0;
});
var_dump(preg_match('/((foo)(bar))(?C42)baz/', 'foobarbaz', $matches));
var_dump($matches);

preg_callout_function(function($info) {
    var_dump($info["callout"]);
    // Abort this path
    return 1;
});
var_dump(preg_match('/foo(?C"name")bar|foo/', 'foobar', $matches));
var_dump($matches);

preg_callout_function(function($info) {
    var_dump($info["callout"]);
    // Abort whole match
    return -1;
});
var_dump(preg_match('/foo(?C"name")bar|foo/', 'foobar', $matches));
var_dump($matches);

?>
--EXPECT--
array(8) {
  ["callout"]=>
  int(42)
  ["subject"]=>
  string(9) "foobarbaz"
  ["captures"]=>
  array(4) {
    [0]=>
    array(2) {
      [0]=>
      NULL
      [1]=>
      int(-1)
    }
    [1]=>
    array(2) {
      [0]=>
      string(6) "foobar"
      [1]=>
      int(0)
    }
    [2]=>
    array(2) {
      [0]=>
      string(3) "foo"
      [1]=>
      int(0)
    }
    [3]=>
    array(2) {
      [0]=>
      string(3) "bar"
      [1]=>
      int(3)
    }
  }
  ["capture_last"]=>
  int(1)
  ["start_match"]=>
  int(0)
  ["current_position"]=>
  int(6)
  ["pattern_position"]=>
  int(18)
  ["next_item_length"]=>
  int(1)
}
int(1)
array(4) {
  [0]=>
  string(9) "foobarbaz"
  [1]=>
  string(6) "foobar"
  [2]=>
  string(3) "foo"
  [3]=>
  string(3) "bar"
}
string(4) "name"
int(1)
array(1) {
  [0]=>
  string(3) "foo"
}
string(4) "name"
int(0)
array(0) {
}
