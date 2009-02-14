--TEST--
array_chunk() - variation 23
--FILE--
<?php
$array = array ("p" => 1, "q" => 2, "r" => 3, "s" => 4, "u" => 5, "v" => 6);
var_dump ($array);
for ($i = 0; $i < (sizeof($array) + 1); $i++) {
	echo "[$i]\n";
	var_dump (@array_chunk ($array, $i));
	var_dump (@array_chunk ($array, $i, TRUE));
	var_dump (@array_chunk ($array, $i, FALSE));
	echo "\n";
}
?>
--EXPECT--
array(6) {
  [u"p"]=>
  int(1)
  [u"q"]=>
  int(2)
  [u"r"]=>
  int(3)
  [u"s"]=>
  int(4)
  [u"u"]=>
  int(5)
  [u"v"]=>
  int(6)
}
[0]
NULL
NULL
NULL

[1]
array(6) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
  [2]=>
  array(1) {
    [0]=>
    int(3)
  }
  [3]=>
  array(1) {
    [0]=>
    int(4)
  }
  [4]=>
  array(1) {
    [0]=>
    int(5)
  }
  [5]=>
  array(1) {
    [0]=>
    int(6)
  }
}
array(6) {
  [0]=>
  array(1) {
    [u"p"]=>
    int(1)
  }
  [1]=>
  array(1) {
    [u"q"]=>
    int(2)
  }
  [2]=>
  array(1) {
    [u"r"]=>
    int(3)
  }
  [3]=>
  array(1) {
    [u"s"]=>
    int(4)
  }
  [4]=>
  array(1) {
    [u"u"]=>
    int(5)
  }
  [5]=>
  array(1) {
    [u"v"]=>
    int(6)
  }
}
array(6) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
  [2]=>
  array(1) {
    [0]=>
    int(3)
  }
  [3]=>
  array(1) {
    [0]=>
    int(4)
  }
  [4]=>
  array(1) {
    [0]=>
    int(5)
  }
  [5]=>
  array(1) {
    [0]=>
    int(6)
  }
}

[2]
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
  [2]=>
  array(2) {
    [0]=>
    int(5)
    [1]=>
    int(6)
  }
}
array(3) {
  [0]=>
  array(2) {
    [u"p"]=>
    int(1)
    [u"q"]=>
    int(2)
  }
  [1]=>
  array(2) {
    [u"r"]=>
    int(3)
    [u"s"]=>
    int(4)
  }
  [2]=>
  array(2) {
    [u"u"]=>
    int(5)
    [u"v"]=>
    int(6)
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
  [2]=>
  array(2) {
    [0]=>
    int(5)
    [1]=>
    int(6)
  }
}

[3]
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [1]=>
  array(3) {
    [0]=>
    int(4)
    [1]=>
    int(5)
    [2]=>
    int(6)
  }
}
array(2) {
  [0]=>
  array(3) {
    [u"p"]=>
    int(1)
    [u"q"]=>
    int(2)
    [u"r"]=>
    int(3)
  }
  [1]=>
  array(3) {
    [u"s"]=>
    int(4)
    [u"u"]=>
    int(5)
    [u"v"]=>
    int(6)
  }
}
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [1]=>
  array(3) {
    [0]=>
    int(4)
    [1]=>
    int(5)
    [2]=>
    int(6)
  }
}

[4]
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
  [1]=>
  array(2) {
    [0]=>
    int(5)
    [1]=>
    int(6)
  }
}
array(2) {
  [0]=>
  array(4) {
    [u"p"]=>
    int(1)
    [u"q"]=>
    int(2)
    [u"r"]=>
    int(3)
    [u"s"]=>
    int(4)
  }
  [1]=>
  array(2) {
    [u"u"]=>
    int(5)
    [u"v"]=>
    int(6)
  }
}
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
  [1]=>
  array(2) {
    [0]=>
    int(5)
    [1]=>
    int(6)
  }
}

[5]
array(2) {
  [0]=>
  array(5) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
  }
  [1]=>
  array(1) {
    [0]=>
    int(6)
  }
}
array(2) {
  [0]=>
  array(5) {
    [u"p"]=>
    int(1)
    [u"q"]=>
    int(2)
    [u"r"]=>
    int(3)
    [u"s"]=>
    int(4)
    [u"u"]=>
    int(5)
  }
  [1]=>
  array(1) {
    [u"v"]=>
    int(6)
  }
}
array(2) {
  [0]=>
  array(5) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
  }
  [1]=>
  array(1) {
    [0]=>
    int(6)
  }
}

[6]
array(1) {
  [0]=>
  array(6) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
    [5]=>
    int(6)
  }
}
array(1) {
  [0]=>
  array(6) {
    [u"p"]=>
    int(1)
    [u"q"]=>
    int(2)
    [u"r"]=>
    int(3)
    [u"s"]=>
    int(4)
    [u"u"]=>
    int(5)
    [u"v"]=>
    int(6)
  }
}
array(1) {
  [0]=>
  array(6) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
    [4]=>
    int(5)
    [5]=>
    int(6)
  }
}
