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
  [u"p"]=>
  unicode(1) "A"
  [u"q"]=>
  unicode(1) "B"
  [u"r"]=>
  unicode(1) "C"
  [u"s"]=>
  unicode(1) "D"
  [u"u"]=>
  unicode(1) "E"
  [u"v"]=>
  unicode(1) "F"
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
    unicode(1) "A"
  }
  [1]=>
  array(1) {
    [0]=>
    unicode(1) "B"
  }
  [2]=>
  array(1) {
    [0]=>
    unicode(1) "C"
  }
  [3]=>
  array(1) {
    [0]=>
    unicode(1) "D"
  }
  [4]=>
  array(1) {
    [0]=>
    unicode(1) "E"
  }
  [5]=>
  array(1) {
    [0]=>
    unicode(1) "F"
  }
}
array(6) {
  [0]=>
  array(1) {
    [u"p"]=>
    unicode(1) "A"
  }
  [1]=>
  array(1) {
    [u"q"]=>
    unicode(1) "B"
  }
  [2]=>
  array(1) {
    [u"r"]=>
    unicode(1) "C"
  }
  [3]=>
  array(1) {
    [u"s"]=>
    unicode(1) "D"
  }
  [4]=>
  array(1) {
    [u"u"]=>
    unicode(1) "E"
  }
  [5]=>
  array(1) {
    [u"v"]=>
    unicode(1) "F"
  }
}
array(6) {
  [0]=>
  array(1) {
    [0]=>
    unicode(1) "A"
  }
  [1]=>
  array(1) {
    [0]=>
    unicode(1) "B"
  }
  [2]=>
  array(1) {
    [0]=>
    unicode(1) "C"
  }
  [3]=>
  array(1) {
    [0]=>
    unicode(1) "D"
  }
  [4]=>
  array(1) {
    [0]=>
    unicode(1) "E"
  }
  [5]=>
  array(1) {
    [0]=>
    unicode(1) "F"
  }
}

[2]
array(3) {
  [0]=>
  array(2) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
  }
  [1]=>
  array(2) {
    [0]=>
    unicode(1) "C"
    [1]=>
    unicode(1) "D"
  }
  [2]=>
  array(2) {
    [0]=>
    unicode(1) "E"
    [1]=>
    unicode(1) "F"
  }
}
array(3) {
  [0]=>
  array(2) {
    [u"p"]=>
    unicode(1) "A"
    [u"q"]=>
    unicode(1) "B"
  }
  [1]=>
  array(2) {
    [u"r"]=>
    unicode(1) "C"
    [u"s"]=>
    unicode(1) "D"
  }
  [2]=>
  array(2) {
    [u"u"]=>
    unicode(1) "E"
    [u"v"]=>
    unicode(1) "F"
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
  }
  [1]=>
  array(2) {
    [0]=>
    unicode(1) "C"
    [1]=>
    unicode(1) "D"
  }
  [2]=>
  array(2) {
    [0]=>
    unicode(1) "E"
    [1]=>
    unicode(1) "F"
  }
}

[3]
array(2) {
  [0]=>
  array(3) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
    [2]=>
    unicode(1) "C"
  }
  [1]=>
  array(3) {
    [0]=>
    unicode(1) "D"
    [1]=>
    unicode(1) "E"
    [2]=>
    unicode(1) "F"
  }
}
array(2) {
  [0]=>
  array(3) {
    [u"p"]=>
    unicode(1) "A"
    [u"q"]=>
    unicode(1) "B"
    [u"r"]=>
    unicode(1) "C"
  }
  [1]=>
  array(3) {
    [u"s"]=>
    unicode(1) "D"
    [u"u"]=>
    unicode(1) "E"
    [u"v"]=>
    unicode(1) "F"
  }
}
array(2) {
  [0]=>
  array(3) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
    [2]=>
    unicode(1) "C"
  }
  [1]=>
  array(3) {
    [0]=>
    unicode(1) "D"
    [1]=>
    unicode(1) "E"
    [2]=>
    unicode(1) "F"
  }
}

[4]
array(2) {
  [0]=>
  array(4) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
    [2]=>
    unicode(1) "C"
    [3]=>
    unicode(1) "D"
  }
  [1]=>
  array(2) {
    [0]=>
    unicode(1) "E"
    [1]=>
    unicode(1) "F"
  }
}
array(2) {
  [0]=>
  array(4) {
    [u"p"]=>
    unicode(1) "A"
    [u"q"]=>
    unicode(1) "B"
    [u"r"]=>
    unicode(1) "C"
    [u"s"]=>
    unicode(1) "D"
  }
  [1]=>
  array(2) {
    [u"u"]=>
    unicode(1) "E"
    [u"v"]=>
    unicode(1) "F"
  }
}
array(2) {
  [0]=>
  array(4) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
    [2]=>
    unicode(1) "C"
    [3]=>
    unicode(1) "D"
  }
  [1]=>
  array(2) {
    [0]=>
    unicode(1) "E"
    [1]=>
    unicode(1) "F"
  }
}

[5]
array(2) {
  [0]=>
  array(5) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
    [2]=>
    unicode(1) "C"
    [3]=>
    unicode(1) "D"
    [4]=>
    unicode(1) "E"
  }
  [1]=>
  array(1) {
    [0]=>
    unicode(1) "F"
  }
}
array(2) {
  [0]=>
  array(5) {
    [u"p"]=>
    unicode(1) "A"
    [u"q"]=>
    unicode(1) "B"
    [u"r"]=>
    unicode(1) "C"
    [u"s"]=>
    unicode(1) "D"
    [u"u"]=>
    unicode(1) "E"
  }
  [1]=>
  array(1) {
    [u"v"]=>
    unicode(1) "F"
  }
}
array(2) {
  [0]=>
  array(5) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
    [2]=>
    unicode(1) "C"
    [3]=>
    unicode(1) "D"
    [4]=>
    unicode(1) "E"
  }
  [1]=>
  array(1) {
    [0]=>
    unicode(1) "F"
  }
}

[6]
array(1) {
  [0]=>
  array(6) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
    [2]=>
    unicode(1) "C"
    [3]=>
    unicode(1) "D"
    [4]=>
    unicode(1) "E"
    [5]=>
    unicode(1) "F"
  }
}
array(1) {
  [0]=>
  array(6) {
    [u"p"]=>
    unicode(1) "A"
    [u"q"]=>
    unicode(1) "B"
    [u"r"]=>
    unicode(1) "C"
    [u"s"]=>
    unicode(1) "D"
    [u"u"]=>
    unicode(1) "E"
    [u"v"]=>
    unicode(1) "F"
  }
}
array(1) {
  [0]=>
  array(6) {
    [0]=>
    unicode(1) "A"
    [1]=>
    unicode(1) "B"
    [2]=>
    unicode(1) "C"
    [3]=>
    unicode(1) "D"
    [4]=>
    unicode(1) "E"
    [5]=>
    unicode(1) "F"
  }
}
