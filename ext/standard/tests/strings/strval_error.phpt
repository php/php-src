--TEST--
Test strval() function : usage variations  - error conditions
--FILE--
<?php
echo "*** Testing strval() : error conditions ***\n";

error_reporting(E_ALL ^ E_NOTICE);

class MyClass
{
    // no toString() method defined
}

// Testing strval with a object which has no toString() method
echo "\n-- Testing strval() function with object which has not toString() method  --\n";
try {
    var_dump( strval(new MyClass()) );
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing strval() : error conditions ***

-- Testing strval() function with object which has not toString() method  --
Object of class MyClass could not be converted to string
