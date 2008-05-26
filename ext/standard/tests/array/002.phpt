--TEST--
Test arsort, asort, krsort, ksort, rsort, and sort
--INI--
precision=14
--FILE--
<?php
require(dirname(__FILE__) . '/data.inc');

function test_sort ($sort_function, $data) {
    echo "\n -- Testing $sort_function() -- \n";
	echo "No second argument:\n";
    $sort_function ($data);
    var_dump ($data);
	echo "Using SORT_REGULAR:\n";
    $sort_function ($data, SORT_REGULAR);
    var_dump ($data);
	echo "Using SORT_NUMERIC:\n";
    $sort_function ($data, SORT_NUMERIC);
    var_dump ($data);
	echo "Using SORT_STRING\n";
    $sort_function ($data, SORT_STRING);
    var_dump ($data);
}

echo "Unsorted data:\n";
var_dump ($data);
foreach (array ('arsort', 'asort', 'krsort', 'ksort', 'rsort', 'sort') as $test_function) {
    test_sort ($test_function, $data);
}

?>
--EXPECTF--
Unsorted data:
array(8) {
  [0]=>
  unicode(3) "PHP"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [u"test"]=>
  int(27)
  [1000]=>
  unicode(4) "test"
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [1001]=>
  unicode(6) "monkey"
  [16777216]=>
  float(-0.33333333333333)
}

 -- Testing arsort() -- 
No second argument:
array(8) {
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [u"test"]=>
  int(27)
  [1000]=>
  unicode(4) "test"
  [1001]=>
  unicode(6) "monkey"
  [5]=>
  unicode(4) "Test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [0]=>
  unicode(3) "PHP"
  [16777216]=>
  float(-0.33333333333333)
}
Using SORT_REGULAR:
array(8) {
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [u"test"]=>
  int(27)
  [1000]=>
  unicode(4) "test"
  [1001]=>
  unicode(6) "monkey"
  [5]=>
  unicode(4) "Test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [0]=>
  unicode(3) "PHP"
  [16777216]=>
  float(-0.33333333333333)
}
Using SORT_NUMERIC:
array(8) {
  [u"test"]=>
  int(27)
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [0]=>
  unicode(3) "PHP"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [1001]=>
  unicode(6) "monkey"
  [5]=>
  unicode(4) "Test"
  [1000]=>
  unicode(4) "test"
  [16777216]=>
  float(-0.33333333333333)
}
Using SORT_STRING

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
array(8) {
  [1000]=>
  unicode(4) "test"
  [1001]=>
  unicode(6) "monkey"
  [5]=>
  unicode(4) "Test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [0]=>
  unicode(3) "PHP"
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [u"test"]=>
  int(27)
  [16777216]=>
  float(-0.33333333333333)
}

 -- Testing asort() -- 
No second argument:
array(8) {
  [16777216]=>
  float(-0.33333333333333)
  [0]=>
  unicode(3) "PHP"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
  [u"test"]=>
  int(27)
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
}
Using SORT_REGULAR:
array(8) {
  [16777216]=>
  float(-0.33333333333333)
  [0]=>
  unicode(3) "PHP"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
  [u"test"]=>
  int(27)
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
}
Using SORT_NUMERIC:
array(8) {
  [16777216]=>
  float(-0.33333333333333)
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
  [5]=>
  unicode(4) "Test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [0]=>
  unicode(3) "PHP"
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [u"test"]=>
  int(27)
}
Using SORT_STRING

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
array(8) {
  [16777216]=>
  float(-0.33333333333333)
  [u"test"]=>
  int(27)
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [0]=>
  unicode(3) "PHP"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
}

 -- Testing krsort() -- 
No second argument:
array(8) {
  [16777216]=>
  float(-0.33333333333333)
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [u"test"]=>
  int(27)
  [0]=>
  unicode(3) "PHP"
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
}
Using SORT_REGULAR:
array(8) {
  [16777216]=>
  float(-0.33333333333333)
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [0]=>
  unicode(3) "PHP"
  [u"test"]=>
  int(27)
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
}
Using SORT_NUMERIC:
array(8) {
  [16777216]=>
  float(-0.33333333333333)
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [u"test"]=>
  int(27)
  [0]=>
  unicode(3) "PHP"
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
}
Using SORT_STRING
array(8) {
  [u"test"]=>
  int(27)
  [5]=>
  unicode(4) "Test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [16777216]=>
  float(-0.33333333333333)
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
  [0]=>
  unicode(3) "PHP"
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
}

 -- Testing ksort() -- 
No second argument:
array(8) {
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [0]=>
  unicode(3) "PHP"
  [u"test"]=>
  int(27)
  [5]=>
  unicode(4) "Test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [1000]=>
  unicode(4) "test"
  [1001]=>
  unicode(6) "monkey"
  [16777216]=>
  float(-0.33333333333333)
}
Using SORT_REGULAR:
array(8) {
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [u"test"]=>
  int(27)
  [0]=>
  unicode(3) "PHP"
  [5]=>
  unicode(4) "Test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [1000]=>
  unicode(4) "test"
  [1001]=>
  unicode(6) "monkey"
  [16777216]=>
  float(-0.33333333333333)
}
Using SORT_NUMERIC:
array(8) {
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [0]=>
  unicode(3) "PHP"
  [u"test"]=>
  int(27)
  [5]=>
  unicode(4) "Test"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [1000]=>
  unicode(4) "test"
  [1001]=>
  unicode(6) "monkey"
  [16777216]=>
  float(-0.33333333333333)
}
Using SORT_STRING
array(8) {
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [0]=>
  unicode(3) "PHP"
  [1000]=>
  unicode(4) "test"
  [1001]=>
  unicode(6) "monkey"
  [16777216]=>
  float(-0.33333333333333)
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [u"test"]=>
  int(27)
}

 -- Testing rsort() -- 
No second argument:
array(8) {
  [0]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [1]=>
  int(27)
  [2]=>
  unicode(4) "test"
  [3]=>
  unicode(6) "monkey"
  [4]=>
  unicode(4) "Test"
  [5]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [6]=>
  unicode(3) "PHP"
  [7]=>
  float(-0.33333333333333)
}
Using SORT_REGULAR:
array(8) {
  [0]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [1]=>
  int(27)
  [2]=>
  unicode(4) "test"
  [3]=>
  unicode(6) "monkey"
  [4]=>
  unicode(4) "Test"
  [5]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [6]=>
  unicode(3) "PHP"
  [7]=>
  float(-0.33333333333333)
}
Using SORT_NUMERIC:
array(8) {
  [0]=>
  int(27)
  [1]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [2]=>
  unicode(3) "PHP"
  [3]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [4]=>
  unicode(6) "monkey"
  [5]=>
  unicode(4) "Test"
  [6]=>
  unicode(4) "test"
  [7]=>
  float(-0.33333333333333)
}
Using SORT_STRING

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
array(8) {
  [0]=>
  unicode(4) "test"
  [1]=>
  unicode(6) "monkey"
  [2]=>
  unicode(4) "Test"
  [3]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [4]=>
  unicode(3) "PHP"
  [5]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [6]=>
  int(27)
  [7]=>
  float(-0.33333333333333)
}

 -- Testing sort() -- 
No second argument:
array(8) {
  [0]=>
  float(-0.33333333333333)
  [1]=>
  unicode(3) "PHP"
  [2]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [3]=>
  unicode(4) "Test"
  [4]=>
  unicode(6) "monkey"
  [5]=>
  unicode(4) "test"
  [6]=>
  int(27)
  [7]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
}
Using SORT_REGULAR:
array(8) {
  [0]=>
  float(-0.33333333333333)
  [1]=>
  unicode(3) "PHP"
  [2]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [3]=>
  unicode(4) "Test"
  [4]=>
  unicode(6) "monkey"
  [5]=>
  unicode(4) "test"
  [6]=>
  int(27)
  [7]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
}
Using SORT_NUMERIC:
array(8) {
  [0]=>
  float(-0.33333333333333)
  [1]=>
  unicode(6) "monkey"
  [2]=>
  unicode(4) "test"
  [3]=>
  unicode(4) "Test"
  [4]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(3) "PHP"
  [6]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [7]=>
  int(27)
}
Using SORT_STRING

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
array(8) {
  [0]=>
  float(-0.33333333333333)
  [1]=>
  int(27)
  [2]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [3]=>
  unicode(3) "PHP"
  [4]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [6]=>
  unicode(6) "monkey"
  [7]=>
  unicode(4) "test"
}
