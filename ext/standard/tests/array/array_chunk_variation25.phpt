--TEST--
array_chunk() - variation 25
--FILE--
<?php
$array = array ("p" => "A", "q" => "B", "r" => "C", "s" => "D", "u" => "E", "v" => "F");
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
  ["p"]=>
  string(1) "A"
  ["q"]=>
  string(1) "B"
  ["r"]=>
  string(1) "C"
  ["s"]=>
  string(1) "D"
  ["u"]=>
  string(1) "E"
  ["v"]=>
  string(1) "F"
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
    string(1) "A"
  }
  [1]=>
  array(1) {
    [0]=>
    string(1) "B"
  }
  [2]=>
  array(1) {
    [0]=>
    string(1) "C"
  }
  [3]=>
  array(1) {
    [0]=>
    string(1) "D"
  }
  [4]=>
  array(1) {
    [0]=>
    string(1) "E"
  }
  [5]=>
  array(1) {
    [0]=>
    string(1) "F"
  }
}
array(6) {
  [0]=>
  array(1) {
    ["p"]=>
    string(1) "A"
  }
  [1]=>
  array(1) {
    ["q"]=>
    string(1) "B"
  }
  [2]=>
  array(1) {
    ["r"]=>
    string(1) "C"
  }
  [3]=>
  array(1) {
    ["s"]=>
    string(1) "D"
  }
  [4]=>
  array(1) {
    ["u"]=>
    string(1) "E"
  }
  [5]=>
  array(1) {
    ["v"]=>
    string(1) "F"
  }
}
array(6) {
  [0]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  [1]=>
  array(1) {
    [0]=>
    string(1) "B"
  }
  [2]=>
  array(1) {
    [0]=>
    string(1) "C"
  }
  [3]=>
  array(1) {
    [0]=>
    string(1) "D"
  }
  [4]=>
  array(1) {
    [0]=>
    string(1) "E"
  }
  [5]=>
  array(1) {
    [0]=>
    string(1) "F"
  }
}

[2]
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "C"
    [1]=>
    string(1) "D"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "E"
    [1]=>
    string(1) "F"
  }
}
array(3) {
  [0]=>
  array(2) {
    ["p"]=>
    string(1) "A"
    ["q"]=>
    string(1) "B"
  }
  [1]=>
  array(2) {
    ["r"]=>
    string(1) "C"
    ["s"]=>
    string(1) "D"
  }
  [2]=>
  array(2) {
    ["u"]=>
    string(1) "E"
    ["v"]=>
    string(1) "F"
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "C"
    [1]=>
    string(1) "D"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "E"
    [1]=>
    string(1) "F"
  }
}

[3]
array(2) {
  [0]=>
  array(3) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
    [2]=>
    string(1) "C"
  }
  [1]=>
  array(3) {
    [0]=>
    string(1) "D"
    [1]=>
    string(1) "E"
    [2]=>
    string(1) "F"
  }
}
array(2) {
  [0]=>
  array(3) {
    ["p"]=>
    string(1) "A"
    ["q"]=>
    string(1) "B"
    ["r"]=>
    string(1) "C"
  }
  [1]=>
  array(3) {
    ["s"]=>
    string(1) "D"
    ["u"]=>
    string(1) "E"
    ["v"]=>
    string(1) "F"
  }
}
array(2) {
  [0]=>
  array(3) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
    [2]=>
    string(1) "C"
  }
  [1]=>
  array(3) {
    [0]=>
    string(1) "D"
    [1]=>
    string(1) "E"
    [2]=>
    string(1) "F"
  }
}

[4]
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
    [2]=>
    string(1) "C"
    [3]=>
    string(1) "D"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "E"
    [1]=>
    string(1) "F"
  }
}
array(2) {
  [0]=>
  array(4) {
    ["p"]=>
    string(1) "A"
    ["q"]=>
    string(1) "B"
    ["r"]=>
    string(1) "C"
    ["s"]=>
    string(1) "D"
  }
  [1]=>
  array(2) {
    ["u"]=>
    string(1) "E"
    ["v"]=>
    string(1) "F"
  }
}
array(2) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
    [2]=>
    string(1) "C"
    [3]=>
    string(1) "D"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "E"
    [1]=>
    string(1) "F"
  }
}

[5]
array(2) {
  [0]=>
  array(5) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
    [2]=>
    string(1) "C"
    [3]=>
    string(1) "D"
    [4]=>
    string(1) "E"
  }
  [1]=>
  array(1) {
    [0]=>
    string(1) "F"
  }
}
array(2) {
  [0]=>
  array(5) {
    ["p"]=>
    string(1) "A"
    ["q"]=>
    string(1) "B"
    ["r"]=>
    string(1) "C"
    ["s"]=>
    string(1) "D"
    ["u"]=>
    string(1) "E"
  }
  [1]=>
  array(1) {
    ["v"]=>
    string(1) "F"
  }
}
array(2) {
  [0]=>
  array(5) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
    [2]=>
    string(1) "C"
    [3]=>
    string(1) "D"
    [4]=>
    string(1) "E"
  }
  [1]=>
  array(1) {
    [0]=>
    string(1) "F"
  }
}

[6]
array(1) {
  [0]=>
  array(6) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
    [2]=>
    string(1) "C"
    [3]=>
    string(1) "D"
    [4]=>
    string(1) "E"
    [5]=>
    string(1) "F"
  }
}
array(1) {
  [0]=>
  array(6) {
    ["p"]=>
    string(1) "A"
    ["q"]=>
    string(1) "B"
    ["r"]=>
    string(1) "C"
    ["s"]=>
    string(1) "D"
    ["u"]=>
    string(1) "E"
    ["v"]=>
    string(1) "F"
  }
}
array(1) {
  [0]=>
  array(6) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "B"
    [2]=>
    string(1) "C"
    [3]=>
    string(1) "D"
    [4]=>
    string(1) "E"
    [5]=>
    string(1) "F"
  }
}
