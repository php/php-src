--TEST--
array_chunk() - variation 19
--FILE--
<?php
$array = array (1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
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
array(10) {
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
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}
[1]
array(10) {
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
  [6]=>
  array(1) {
    [0]=>
    int(7)
  }
  [7]=>
  array(1) {
    [0]=>
    int(8)
  }
  [8]=>
  array(1) {
    [0]=>
    int(9)
  }
  [9]=>
  array(1) {
    [0]=>
    int(10)
  }
}
array(10) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [1]=>
    int(2)
  }
  [2]=>
  array(1) {
    [2]=>
    int(3)
  }
  [3]=>
  array(1) {
    [3]=>
    int(4)
  }
  [4]=>
  array(1) {
    [4]=>
    int(5)
  }
  [5]=>
  array(1) {
    [5]=>
    int(6)
  }
  [6]=>
  array(1) {
    [6]=>
    int(7)
  }
  [7]=>
  array(1) {
    [7]=>
    int(8)
  }
  [8]=>
  array(1) {
    [8]=>
    int(9)
  }
  [9]=>
  array(1) {
    [9]=>
    int(10)
  }
}
array(10) {
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
  [6]=>
  array(1) {
    [0]=>
    int(7)
  }
  [7]=>
  array(1) {
    [0]=>
    int(8)
  }
  [8]=>
  array(1) {
    [0]=>
    int(9)
  }
  [9]=>
  array(1) {
    [0]=>
    int(10)
  }
}

[2]
array(5) {
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
  [3]=>
  array(2) {
    [0]=>
    int(7)
    [1]=>
    int(8)
  }
  [4]=>
  array(2) {
    [0]=>
    int(9)
    [1]=>
    int(10)
  }
}
array(5) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
  [2]=>
  array(2) {
    [4]=>
    int(5)
    [5]=>
    int(6)
  }
  [3]=>
  array(2) {
    [6]=>
    int(7)
    [7]=>
    int(8)
  }
  [4]=>
  array(2) {
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
}
array(5) {
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
  [3]=>
  array(2) {
    [0]=>
    int(7)
    [1]=>
    int(8)
  }
  [4]=>
  array(2) {
    [0]=>
    int(9)
    [1]=>
    int(10)
  }
}

[3]
array(4) {
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
  [2]=>
  array(3) {
    [0]=>
    int(7)
    [1]=>
    int(8)
    [2]=>
    int(9)
  }
  [3]=>
  array(1) {
    [0]=>
    int(10)
  }
}
array(4) {
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
    [3]=>
    int(4)
    [4]=>
    int(5)
    [5]=>
    int(6)
  }
  [2]=>
  array(3) {
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
  }
  [3]=>
  array(1) {
    [9]=>
    int(10)
  }
}
array(4) {
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
  [2]=>
  array(3) {
    [0]=>
    int(7)
    [1]=>
    int(8)
    [2]=>
    int(9)
  }
  [3]=>
  array(1) {
    [0]=>
    int(10)
  }
}

[4]
array(3) {
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
  array(4) {
    [0]=>
    int(5)
    [1]=>
    int(6)
    [2]=>
    int(7)
    [3]=>
    int(8)
  }
  [2]=>
  array(2) {
    [0]=>
    int(9)
    [1]=>
    int(10)
  }
}
array(3) {
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
  array(4) {
    [4]=>
    int(5)
    [5]=>
    int(6)
    [6]=>
    int(7)
    [7]=>
    int(8)
  }
  [2]=>
  array(2) {
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
}
array(3) {
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
  array(4) {
    [0]=>
    int(5)
    [1]=>
    int(6)
    [2]=>
    int(7)
    [3]=>
    int(8)
  }
  [2]=>
  array(2) {
    [0]=>
    int(9)
    [1]=>
    int(10)
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
  array(5) {
    [0]=>
    int(6)
    [1]=>
    int(7)
    [2]=>
    int(8)
    [3]=>
    int(9)
    [4]=>
    int(10)
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
  array(5) {
    [5]=>
    int(6)
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
    [9]=>
    int(10)
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
  array(5) {
    [0]=>
    int(6)
    [1]=>
    int(7)
    [2]=>
    int(8)
    [3]=>
    int(9)
    [4]=>
    int(10)
  }
}

[6]
array(2) {
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
  [1]=>
  array(4) {
    [0]=>
    int(7)
    [1]=>
    int(8)
    [2]=>
    int(9)
    [3]=>
    int(10)
  }
}
array(2) {
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
  [1]=>
  array(4) {
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
}
array(2) {
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
  [1]=>
  array(4) {
    [0]=>
    int(7)
    [1]=>
    int(8)
    [2]=>
    int(9)
    [3]=>
    int(10)
  }
}

[7]
array(2) {
  [0]=>
  array(7) {
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
    [6]=>
    int(7)
  }
  [1]=>
  array(3) {
    [0]=>
    int(8)
    [1]=>
    int(9)
    [2]=>
    int(10)
  }
}
array(2) {
  [0]=>
  array(7) {
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
    [6]=>
    int(7)
  }
  [1]=>
  array(3) {
    [7]=>
    int(8)
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
}
array(2) {
  [0]=>
  array(7) {
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
    [6]=>
    int(7)
  }
  [1]=>
  array(3) {
    [0]=>
    int(8)
    [1]=>
    int(9)
    [2]=>
    int(10)
  }
}

[8]
array(2) {
  [0]=>
  array(8) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
  }
  [1]=>
  array(2) {
    [0]=>
    int(9)
    [1]=>
    int(10)
  }
}
array(2) {
  [0]=>
  array(8) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
  }
  [1]=>
  array(2) {
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
}
array(2) {
  [0]=>
  array(8) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
  }
  [1]=>
  array(2) {
    [0]=>
    int(9)
    [1]=>
    int(10)
  }
}

[9]
array(2) {
  [0]=>
  array(9) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
  }
  [1]=>
  array(1) {
    [0]=>
    int(10)
  }
}
array(2) {
  [0]=>
  array(9) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
  }
  [1]=>
  array(1) {
    [9]=>
    int(10)
  }
}
array(2) {
  [0]=>
  array(9) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
  }
  [1]=>
  array(1) {
    [0]=>
    int(10)
  }
}

[10]
array(1) {
  [0]=>
  array(10) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
}
array(1) {
  [0]=>
  array(10) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
}
array(1) {
  [0]=>
  array(10) {
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
    [6]=>
    int(7)
    [7]=>
    int(8)
    [8]=>
    int(9)
    [9]=>
    int(10)
  }
}
