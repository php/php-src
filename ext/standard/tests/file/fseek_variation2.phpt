--TEST--
Test fseek() function : usage variations  - different types for whence
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : proto int fseek(resource fp, int offset [, int whence])
 * Description: Seek on a file pointer 
 * Source code: ext/standard/file.c
 * Alias to functions: gzseek
 */

echo "*** Testing fseek() : usage variations ***\n";
error_reporting(E_ALL & ~E_NOTICE);
$fp = fopen(__FILE__, 'r');
$offset = 3;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

      // outside of whence range
      -100,
      100,

      // float data
      10.5,
      -10.5,
      10.1234567e10,
      10.7654321E-10,
      .5,

      // null data
      NULL,
      null,

      // boolean data
      true,
      false,
      TRUE,
      FALSE,

      // empty data
      "",
      '',

      // string data
      "string",
      'string',

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
);

// loop through each element of the array for whence

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( fseek($fp, $offset, $value) );
      var_dump( ftell($fp));
};

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing fseek() : usage variations ***

Arg value -100 
int(-1)
int(0)

Arg value 100 
int(-1)
int(0)

Arg value 10.5 
int(-1)
int(0)

Arg value -10.5 
int(-1)
int(0)

Arg value 101234567000 
int(-1)
int(0)

Arg value 1.07654321E-9 
int(0)
int(3)

Arg value 0.5 
int(0)
int(3)

Arg value  
int(0)
int(3)

Arg value  
int(0)
int(3)

Arg value 1 
int(0)
int(6)

Arg value  
int(0)
int(3)

Arg value 1 
int(0)
int(6)

Arg value  
int(0)
int(3)

Arg value  

Warning: fseek() expects parameter 3 to be long, string given in %s on line %d
bool(false)
int(3)

Arg value  

Warning: fseek() expects parameter 3 to be long, string given in %s on line %d
bool(false)
int(3)

Arg value string 

Warning: fseek() expects parameter 3 to be long, string given in %s on line %d
bool(false)
int(3)

Arg value string 

Warning: fseek() expects parameter 3 to be long, string given in %s on line %d
bool(false)
int(3)

Arg value  
int(0)
int(3)

Arg value  
int(0)
int(3)
Done

