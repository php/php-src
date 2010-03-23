--TEST--
Test session_decode() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : string session_decode(void)
 * Description : Decodes session data from a string
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_decode() : error functionality ***\n";
$data = "foo|a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}guff|R:1;blah|R:1;";

var_dump(session_start());
for($index = 0; $index < strlen($data); $index++) {
    echo "\n-- Iteration $index --\n";
    $encoded = substr($data, 0, $index);
    var_dump(session_decode($encoded));
    var_dump($_SESSION);
};

var_dump(session_destroy());
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_decode() : error functionality ***
bool(true)

-- Iteration 0 --
bool(true)
array(0) {
}

-- Iteration 1 --
bool(true)
array(0) {
}

-- Iteration 2 --
bool(true)
array(0) {
}

-- Iteration 3 --
bool(true)
array(0) {
}

-- Iteration 4 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 5 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 6 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 7 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 8 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 9 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 10 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 11 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 12 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 13 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 14 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 15 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 16 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 17 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 18 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 19 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 20 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 21 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 22 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 23 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 24 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 25 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 26 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 27 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 28 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 29 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 30 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 31 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 32 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 33 --
bool(true)
array(1) {
  ["foo"]=>
  NULL
}

-- Iteration 34 --
bool(true)
array(1) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 35 --
bool(true)
array(1) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 36 --
bool(true)
array(1) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 37 --
bool(true)
array(1) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 38 --
bool(true)
array(1) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 39 --
bool(true)
array(2) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  NULL
}

-- Iteration 40 --
bool(true)
array(2) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  NULL
}

-- Iteration 41 --
bool(true)
array(2) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  NULL
}

-- Iteration 42 --
bool(true)
array(2) {
  ["foo"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  NULL
}

-- Iteration 43 --
bool(true)
array(2) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 44 --
bool(true)
array(2) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 45 --
bool(true)
array(2) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 46 --
bool(true)
array(2) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 47 --
bool(true)
array(2) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Iteration 48 --
bool(true)
array(3) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["blah"]=>
  NULL
}

-- Iteration 49 --
bool(true)
array(3) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["blah"]=>
  NULL
}

-- Iteration 50 --
bool(true)
array(3) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["blah"]=>
  NULL
}

-- Iteration 51 --
bool(true)
array(3) {
  ["foo"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["guff"]=>
  &array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["blah"]=>
  NULL
}
bool(true)
Done

