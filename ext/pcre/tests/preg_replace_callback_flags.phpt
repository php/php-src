--TEST--
Support for flags in preg_replace_callback(_array)
--FILE--
<?php

var_dump(preg_replace_callback('/./', function($matches) {
    var_dump($matches);
    return $matches[0][0];
}, 'abc', -1, $count, PREG_OFFSET_CAPTURE));
echo "\n";

var_dump(preg_replace_callback_array(
    ['/./' => function($matches) {
        var_dump($matches);
        return $matches[0][0];
    }],
    'abc', -1, $count, PREG_OFFSET_CAPTURE)
);
echo "\n";

var_dump(preg_replace_callback('/(a)|(b)/', function($matches) {
    var_dump($matches);
    return $matches[0];
}, 'abc', -1, $count, PREG_UNMATCHED_AS_NULL));
echo "\n";

var_dump(preg_replace_callback_array(
    ['/(a)|(b)/' => function($matches) {
        var_dump($matches);
        return $matches[0];
    }],
    'abc', -1, $count, PREG_UNMATCHED_AS_NULL)
);
echo "\n";

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    int(0)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    int(2)
  }
}
string(3) "abc"

array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    int(0)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    int(2)
  }
}
string(3) "abc"

array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "a"
  [2]=>
  NULL
}
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  NULL
  [2]=>
  string(1) "b"
}
string(3) "abc"

array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "a"
  [2]=>
  NULL
}
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  NULL
  [2]=>
  string(1) "b"
}
string(3) "abc"
