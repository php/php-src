--TEST--
Test get_defined_vars() function
--FILE--
<?php
/* Prototype: array get_defined_vars  ( void  )
   Description: This function returns a multidimensional array containing a list of all defined 
   variables, be them environment, server or user-defined variables, within the scope that 
   get_defined_vars() is called.
*/

echo "Simple testcase for get_defined_vars() function\n\n";

function f1() {
  echo "\n-- Function f1() called --\n"; 
  $vars = get_defined_vars();
  
  if (count($vars) != 0) {
	 echo "TEST FAILED\n"; 
  } 
  
  echo "\n-- ..define some local variables --\n"; 
  $i = 123;
  $f = 123.456;
  $b = false;
  $s = "Hello World"; 
  $arr = array(1,2,3,4);
  var_dump( get_defined_vars() ); 
  f2();
}

function f2() {
  echo "\n -- Function f2() called --\n"; 
  $vars= get_defined_vars();
  
  if (count($vars) != 0) {
	 echo "TEST FAILED\n"; 
  }
   
  echo "\n-- ...define some variables --\n"; 
  $i = 456;
  $f = 456.678;
  $b = true;
  $s = "Goodnight"; 
  $arr = array("foo", "bar");  
  var_dump( get_defined_vars() );
   
  echo "\n-- ...define some more variables --\n";
  $i1 = 456;
  $f1 = 456.678;
  $b1 = true;
  var_dump( get_defined_vars() );

}

echo "\n-- Get variables at global scope --\n";
$vars = get_defined_vars();

if (count($vars) == 0) {
   echo "TEST FAILED - Global variables missing at global scope\n"; 
}   

// call a function
f1();

?>
===DONE===
--EXPECT--
Simple testcase for get_defined_vars() function


-- Get variables at global scope --

-- Function f1() called --

-- ..define some local variables --
array(6) {
  [u"vars"]=>
  array(0) {
  }
  [u"i"]=>
  int(123)
  [u"f"]=>
  float(123.456)
  [u"b"]=>
  bool(false)
  [u"s"]=>
  unicode(11) "Hello World"
  [u"arr"]=>
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
}

 -- Function f2() called --

-- ...define some variables --
array(6) {
  [u"vars"]=>
  array(0) {
  }
  [u"i"]=>
  int(456)
  [u"f"]=>
  float(456.678)
  [u"b"]=>
  bool(true)
  [u"s"]=>
  unicode(9) "Goodnight"
  [u"arr"]=>
  array(2) {
    [0]=>
    unicode(3) "foo"
    [1]=>
    unicode(3) "bar"
  }
}

-- ...define some more variables --
array(9) {
  [u"vars"]=>
  array(0) {
  }
  [u"i"]=>
  int(456)
  [u"f"]=>
  float(456.678)
  [u"b"]=>
  bool(true)
  [u"s"]=>
  unicode(9) "Goodnight"
  [u"arr"]=>
  array(2) {
    [0]=>
    unicode(3) "foo"
    [1]=>
    unicode(3) "bar"
  }
  [u"i1"]=>
  int(456)
  [u"f1"]=>
  float(456.678)
  [u"b1"]=>
  bool(true)
}
===DONE===