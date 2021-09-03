--TEST--
Indexing - various special cases.
--FILE--
<?php
echo "*** Indexing - Testing value assignment with key ***\n";
$array=array(1);
$testvalues=array(null, 0, 1, true, false,'',' ',0.1,array());

foreach ($testvalues as $testvalue) {
    try {
        $testvalue['foo']=$array;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($testvalue);
}
echo "\n*** Indexing - Testing reference assignment with key ***\n";

$testvalues=array(null, 0, 1, true, false,0.1,array());

foreach ($testvalues as $testvalue) {
    try {
        $testvalue['foo']=&$array;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($testvalue);
}
echo "*** Indexing - Testing value assignment no key ***\n";
$array=array(1);
$testvalues=array(null, 0, 1, true, false,0.1,array());

foreach ($testvalues as $testvalue) {
    try {
        $testvalue[]=$array;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump ($testvalue);
}
echo "\n*** Indexing - Testing reference assignment no key ***\n";

$testvalues=array(null, 0, 1, true, false,0.1,array());

foreach ($testvalues as $testvalue) {
    try {
        $testvalue[]=&$array;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump ($testvalue);
}

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
Cannot use a scalar value as an array
int(0)
Cannot use a scalar value as an array
int(1)
Cannot use a scalar value as an array
bool(true)

Deprecated: Automatic conversion of false to array is deprecated in %s
array(1) {
  ["foo"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
Cannot access offset of type string on string
string(0) ""
Cannot access offset of type string on string
string(1) " "
Cannot use a scalar value as an array
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
Cannot use a scalar value as an array
int(0)
Cannot use a scalar value as an array
int(1)
Cannot use a scalar value as an array
bool(true)

Deprecated: Automatic conversion of false to array is deprecated in %s
array(1) {
  ["foo"]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
Cannot use a scalar value as an array
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
Cannot use a scalar value as an array
int(0)
Cannot use a scalar value as an array
int(1)
Cannot use a scalar value as an array
bool(true)

Deprecated: Automatic conversion of false to array is deprecated in %s
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}
Cannot use a scalar value as an array
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
Cannot use a scalar value as an array
int(0)
Cannot use a scalar value as an array
int(1)
Cannot use a scalar value as an array
bool(true)

Deprecated: Automatic conversion of false to array is deprecated in %s
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
Cannot use a scalar value as an array
float(0.1)
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    int(1)
  }
}
