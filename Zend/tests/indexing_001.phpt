--TEST--
Indexing - various special cases.
--FILE--
<?php
echo "*** Indexing - Testing value assignment with key ***\n";
$array=array(1);
$testvalues=array(null, 0, 1, true, false,'',' ',0.1,array());

foreach ($testvalues as $testvalue) {
	$testvalue['foo']=$array;
	var_dump ($testvalue);
}
echo "\n*** Indexing - Testing reference assignment with key ***\n";

$testvalues=array(null, 0, 1, true, false,'',0.1,array());

foreach ($testvalues as $testvalue) {
	$testvalue['foo']=&$array;
	var_dump ($testvalue);
}
echo "*** Indexing - Testing value assignment no key ***\n";
$array=array(1);
$testvalues=array(null, 0, 1, true, false,'',0.1,array());

foreach ($testvalues as $testvalue) {
	$testvalue[]=$array;
	var_dump ($testvalue);
}
echo "\n*** Indexing - Testing reference assignment no key ***\n";

$testvalues=array(null, 0, 1, true, false,'',0.1,array());

foreach ($testvalues as $testvalue) {
	$testvalue[]=&$array;
	var_dump ($testvalue);
}


echo "\nDone";
?>
--EXPECTF--
*** Indexing - Testing value assignment with key ***
array(1) {
  ["foo"]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
int(0)

Warning: Cannot use a scalar value as an array in %s on line %d
int(1)

Warning: Cannot use a scalar value as an array in %s on line %d
bool(true)
array(1) {
  ["foo"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  ["foo"]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Warning: Illegal string offset 'foo' in %s on line %d

Notice: Array to string conversion in %s on line %d
string(1) "A"

Warning: Cannot use a scalar value as an array in %s on line %d
float(0.1)
array(1) {
  ["foo"]=>
  array(1) {
    [0]=>
    int(1)
  }
}

*** Indexing - Testing reference assignment with key ***
array(1) {
  ["foo"]=>
  &array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
int(0)

Warning: Cannot use a scalar value as an array in %s on line %d
int(1)

Warning: Cannot use a scalar value as an array in %s on line %d
bool(true)
array(1) {
  ["foo"]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  ["foo"]=>
  &array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
float(0.1)
array(1) {
  ["foo"]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
*** Indexing - Testing value assignment no key ***
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
int(0)

Warning: Cannot use a scalar value as an array in %s on line %d
int(1)

Warning: Cannot use a scalar value as an array in %s on line %d
bool(true)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
float(0.1)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}

*** Indexing - Testing reference assignment no key ***
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
int(0)

Warning: Cannot use a scalar value as an array in %s on line %d
int(1)

Warning: Cannot use a scalar value as an array in %s on line %d
bool(true)
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %s on line %d
float(0.1)
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    int(1)
  }
}

Done