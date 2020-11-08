--TEST--
array_chunk() - variation 18
--FILE--
<?php
$array = array (0, 1, 2, 3, 4);
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
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}
[1]
array(5) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
  [2]=>
  array(1) {
    [0]=>
    int(2)
  }
  [3]=>
  array(1) {
    [0]=>
    int(3)
  }
  [4]=>
  array(1) {
    [0]=>
    int(4)
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  array(1) {
    [1]=>
    int(1)
  }
  [2]=>
  array(1) {
    [2]=>
    int(2)
  }
  [3]=>
  array(1) {
    [3]=>
    int(3)
  }
  [4]=>
  array(1) {
    [4]=>
    int(4)
  }
}
array(5) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
  [2]=>
  array(1) {
    [0]=>
    int(2)
  }
  [3]=>
  array(1) {
    [0]=>
    int(3)
  }
  [4]=>
  array(1) {
    [0]=>
    int(4)
  }
}

[2]
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
  [2]=>
  array(1) {
    [0]=>
    int(4)
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
  [1]=>
  array(2) {
    [2]=>
    int(2)
    [3]=>
    int(3)
  }
  [2]=>
  array(1) {
    [4]=>
    int(4)
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(3)
  }
  [2]=>
  array(1) {
    [0]=>
    int(4)
  }
}

[3]
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
}
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
  }
  [1]=>
  array(2) {
    [3]=>
    int(3)
    [4]=>
    int(4)
  }
}
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
}

[4]
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    int(3)
  }
  [1]=>
  array(1) {
    [0]=>
    int(4)
  }
}
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    int(3)
  }
  [1]=>
  array(1) {
    [4]=>
    int(4)
  }
}
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    int(3)
  }
  [1]=>
  array(1) {
    [0]=>
    int(4)
  }
}

[5]
array(1) {
  [0]=>
  array(5) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    int(3)
    [4]=>
    int(4)
  }
}
array(1) {
  [0]=>
  array(5) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    int(3)
    [4]=>
    int(4)
  }
}
array(1) {
  [0]=>
  array(5) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    int(3)
    [4]=>
    int(4)
  }
}
