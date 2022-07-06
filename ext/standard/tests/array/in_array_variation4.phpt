--TEST--
Test in_array() function : usage variations - haystack as resource/multi dimensional array
--FILE--
<?php
/* Test in_array() with haystack as resource and multidimensional arrays */

/* checking for Resources */
echo "*** Testing resource type with in_array() ***\n";
//file type resource
$file_handle = fopen(__FILE__, "r");

//directory type resource
$dir_handle = opendir( __DIR__ );

//store resources in array for comparison.
$resources = array($file_handle, $dir_handle);

// search for resource type in the resource array
var_dump( in_array($file_handle, $resources, true) );
//checking for (int) type resource
var_dump( in_array((int)$dir_handle, $resources, true) );

/* Miscellenous input check  */
echo "\n*** Testing miscelleneos inputs with in_array() ***\n";
//matching "Good" in array(0,"hello"), result:true in loose type check
var_dump( in_array("Good", array(0,"hello")) );
//false in strict mode
var_dump( in_array("Good", array(0,"hello"), TRUE) );

//matching integer 0 in array("this"), result:true in loose type check
var_dump( in_array(0, array("this")) );
// false in strict mode
var_dump( in_array(0, array("this")),TRUE );

//matching string "this" in array(0), result:true in loose type check
var_dump( in_array("this", array(0)) );
// false in stric mode
var_dump( in_array("this", array(0), TRUE) );

//checking for type FALSE in multidimensional array with loose checking, result:false in loose type check
var_dump( in_array(FALSE,
                   array("a"=> TRUE, "b"=> TRUE,
                         array("c"=> TRUE, "d"=>TRUE)
                        )
                  )
        );

//matching string having integer in beginning, result:true in loose type check
var_dump( in_array('123abc', array(123)) );
var_dump( in_array('123abc', array(123), TRUE) ); // false in strict mode

echo "Done\n";
?>
--EXPECT--
*** Testing resource type with in_array() ***
bool(true)
bool(false)

*** Testing miscelleneos inputs with in_array() ***
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done
