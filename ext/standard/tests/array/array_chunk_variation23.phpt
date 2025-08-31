--TEST--
array_chunk() - variation 23
--FILE--
<?php
$array = array ("p" => 1, "q" => 2, "r" => 3, "s" => 4, "u" => 5, "v" => 6);
var_dump ($array);
for ($i = 1; $i < (sizeof($array) + 1); $i++) {
    echo "[$i]\n";
    var_dump (array_chunk ($array, $i));
    var_dump (array_chunk ($array, $i, TRUE));
    var_dump (array_chunk ($array, $i, FALSE));
    echo "\n";
}
?>
--EXPECT--
array(6) {
  ["p"]=>
  int(1)
  ["q"]=>
  int(2)
  ["r"]=>
  int(3)
  ["s"]=>
  int(4)
  ["u"]=>
  int(5)
  ["v"]=>
  int(6)
}
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
    ["p"]=>
    int(1)
  }
  [1]=>
  array(1) {
    ["q"]=>
    int(2)
  }
  [2]=>
  array(1) {
    ["r"]=>
    int(3)
  }
  [3]=>
  array(1) {
    ["s"]=>
    int(4)
  }
  [4]=>
  array(1) {
    ["u"]=>
    int(5)
  }
  [5]=>
  array(1) {
    ["v"]=>
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
    ["p"]=>
    int(1)
    ["q"]=>
    int(2)
  }
  [1]=>
  array(2) {
    ["r"]=>
    int(3)
    ["s"]=>
    int(4)
  }
  [2]=>
  array(2) {
    ["u"]=>
    int(5)
    ["v"]=>
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
    ["p"]=>
    int(1)
    ["q"]=>
    int(2)
    ["r"]=>
    int(3)
  }
  [1]=>
  array(3) {
    ["s"]=>
    int(4)
    ["u"]=>
    int(5)
    ["v"]=>
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
    ["p"]=>
    int(1)
    ["q"]=>
    int(2)
    ["r"]=>
    int(3)
    ["s"]=>
    int(4)
  }
  [1]=>
  array(2) {
    ["u"]=>
    int(5)
    ["v"]=>
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
    ["p"]=>
    int(1)
    ["q"]=>
    int(2)
    ["r"]=>
    int(3)
    ["s"]=>
    int(4)
    ["u"]=>
    int(5)
  }
  [1]=>
  array(1) {
    ["v"]=>
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
    ["p"]=>
    int(1)
    ["q"]=>
    int(2)
    ["r"]=>
    int(3)
    ["s"]=>
    int(4)
    ["u"]=>
    int(5)
    ["v"]=>
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
