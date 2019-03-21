--TEST--
PREG_LENGTH_CAPTURE mode
--FILE--
<?php

$regex = '/(foo)(bar)?(baz)?/';
$subject = 'foobaz foobar';

echo "preg_match():\n";

echo "LENGTH_CAPTURE:\n";
preg_match($regex, $subject, $matches, PREG_LENGTH_CAPTURE);
var_dump($matches);

echo "LENGTH_CAPTURE | UNMATCHED_AS_NULL:\n";
preg_match($regex, $subject, $matches, PREG_LENGTH_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_dump($matches);

echo "LENGTH_CAPTURE | OFFSET_CAPTURE:\n";
preg_match($regex, $subject, $matches, PREG_LENGTH_CAPTURE | PREG_OFFSET_CAPTURE);
var_dump($matches);

echo "LENGTH_CAPTURE | OFFSET_CAPTURE | UNMATCHED_AS_NULL:\n";
preg_match(
    $regex, $subject, $matches, PREG_LENGTH_CAPTURE | PREG_OFFSET_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_dump($matches);

echo "\npreg_match_all():\n";

echo "LENGTH_CAPTURE:\n";
preg_match_all($regex, $subject, $matches, PREG_LENGTH_CAPTURE);
var_dump($matches);

echo "LENGTH_CAPTURE | UNMATCHED_AS_NULL:\n";
preg_match_all($regex, $subject, $matches, PREG_LENGTH_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_dump($matches);

echo "LENGTH_CAPTURE | OFFSET_CAPTURE:\n";
preg_match_all($regex, $subject, $matches, PREG_LENGTH_CAPTURE | PREG_OFFSET_CAPTURE);
var_dump($matches);

echo "LENGTH_CAPTURE | OFFSET_CAPTURE | UNMATCHED_AS_NULL:\n";
preg_match_all(
    $regex, $subject, $matches, PREG_LENGTH_CAPTURE | PREG_OFFSET_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_dump($matches);

echo "\npreg_match_all() in PREG_SET_ORDER:\n";

echo "LENGTH_CAPTURE:\n";
preg_match_all($regex, $subject, $matches, PREG_SET_ORDER | PREG_LENGTH_CAPTURE);
var_dump($matches);

echo "LENGTH_CAPTURE | UNMATCHED_AS_NULL:\n";
preg_match_all(
    $regex, $subject, $matches, PREG_SET_ORDER | PREG_LENGTH_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_dump($matches);

echo "LENGTH_CAPTURE | OFFSET_CAPTURE:\n";
preg_match_all(
    $regex, $subject, $matches, PREG_SET_ORDER | PREG_LENGTH_CAPTURE | PREG_OFFSET_CAPTURE);
var_dump($matches);

echo "LENGTH_CAPTURE | OFFSET_CAPTURE | UNMATCHED_AS_NULL:\n";
preg_match_all(
    $regex, $subject, $matches,
    PREG_SET_ORDER | PREG_LENGTH_CAPTURE | PREG_OFFSET_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_dump($matches);

?>
--EXPECT--
preg_match():
LENGTH_CAPTURE:
array(4) {
  [0]=>
  int(6)
  [1]=>
  int(3)
  [2]=>
  int(0)
  [3]=>
  int(3)
}
LENGTH_CAPTURE | UNMATCHED_AS_NULL:
array(4) {
  [0]=>
  int(6)
  [1]=>
  int(3)
  [2]=>
  NULL
  [3]=>
  int(3)
}
LENGTH_CAPTURE | OFFSET_CAPTURE:
array(4) {
  [0]=>
  array(2) {
    [0]=>
    int(6)
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(0)
  }
  [2]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(-1)
  }
  [3]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(3)
  }
}
LENGTH_CAPTURE | OFFSET_CAPTURE | UNMATCHED_AS_NULL:
array(4) {
  [0]=>
  array(2) {
    [0]=>
    int(6)
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(0)
  }
  [2]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    int(-1)
  }
  [3]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(3)
  }
}

preg_match_all():
LENGTH_CAPTURE:
array(4) {
  [0]=>
  array(2) {
    [0]=>
    int(6)
    [1]=>
    int(6)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(3)
  }
  [2]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(3)
  }
  [3]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(0)
  }
}
LENGTH_CAPTURE | UNMATCHED_AS_NULL:
array(4) {
  [0]=>
  array(2) {
    [0]=>
    int(6)
    [1]=>
    int(6)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(3)
  }
  [2]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    int(3)
  }
  [3]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    NULL
  }
}
LENGTH_CAPTURE | OFFSET_CAPTURE:
array(4) {
  [0]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      int(6)
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      int(6)
      [1]=>
      int(7)
    }
  }
  [1]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(7)
    }
  }
  [2]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      int(0)
      [1]=>
      int(-1)
    }
    [1]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(10)
    }
  }
  [3]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(3)
    }
    [1]=>
    array(2) {
      [0]=>
      int(0)
      [1]=>
      int(-1)
    }
  }
}
LENGTH_CAPTURE | OFFSET_CAPTURE | UNMATCHED_AS_NULL:
array(4) {
  [0]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      int(6)
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      int(6)
      [1]=>
      int(7)
    }
  }
  [1]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(7)
    }
  }
  [2]=>
  array(2) {
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
      int(3)
      [1]=>
      int(10)
    }
  }
  [3]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(3)
    }
    [1]=>
    array(2) {
      [0]=>
      NULL
      [1]=>
      int(-1)
    }
  }
}

preg_match_all() in PREG_SET_ORDER:
LENGTH_CAPTURE:
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(6)
    [1]=>
    int(3)
    [2]=>
    int(0)
    [3]=>
    int(3)
  }
  [1]=>
  array(3) {
    [0]=>
    int(6)
    [1]=>
    int(3)
    [2]=>
    int(3)
  }
}
LENGTH_CAPTURE | UNMATCHED_AS_NULL:
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(6)
    [1]=>
    int(3)
    [2]=>
    NULL
    [3]=>
    int(3)
  }
  [1]=>
  array(4) {
    [0]=>
    int(6)
    [1]=>
    int(3)
    [2]=>
    int(3)
    [3]=>
    NULL
  }
}
LENGTH_CAPTURE | OFFSET_CAPTURE:
array(2) {
  [0]=>
  array(4) {
    [0]=>
    array(2) {
      [0]=>
      int(6)
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(0)
    }
    [2]=>
    array(2) {
      [0]=>
      int(0)
      [1]=>
      int(-1)
    }
    [3]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(3)
    }
  }
  [1]=>
  array(3) {
    [0]=>
    array(2) {
      [0]=>
      int(6)
      [1]=>
      int(7)
    }
    [1]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(7)
    }
    [2]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(10)
    }
  }
}
LENGTH_CAPTURE | OFFSET_CAPTURE | UNMATCHED_AS_NULL:
array(2) {
  [0]=>
  array(4) {
    [0]=>
    array(2) {
      [0]=>
      int(6)
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(0)
    }
    [2]=>
    array(2) {
      [0]=>
      NULL
      [1]=>
      int(-1)
    }
    [3]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(3)
    }
  }
  [1]=>
  array(4) {
    [0]=>
    array(2) {
      [0]=>
      int(6)
      [1]=>
      int(7)
    }
    [1]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(7)
    }
    [2]=>
    array(2) {
      [0]=>
      int(3)
      [1]=>
      int(10)
    }
    [3]=>
    array(2) {
      [0]=>
      NULL
      [1]=>
      int(-1)
    }
  }
}
