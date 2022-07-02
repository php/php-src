--TEST--
Test array_search() function : usage variations - haystack as resource/multi dimensional array
--FILE--
<?php
/* checking for Resources */
echo "*** Testing resource type with array_search() ***\n";
//file type resource
$file_handle = fopen(__FILE__, "r");

//directory type resource
$dir_handle = opendir( __DIR__ );

//store resources in array for comparison.
$resources = array($file_handle, $dir_handle);

// search for resource type in the resource array
var_dump( array_search($file_handle, $resources, true) );
//checking for (int) type resource
var_dump( array_search((int)$dir_handle, $resources, true) );

/* Miscellenous input check  */
echo "\n*** Testing miscelleneos inputs with array_search() ***\n";
//matching "Good" in array(0,"hello"), result:true in loose type check
var_dump( array_search("Good", array(0,"hello")) );
//false in strict mode
var_dump( array_search("Good", array(0,"hello"), TRUE) );

//matching integer 0 in array("this"), result:true in loose type check
var_dump( array_search(0, array("this")) );
// false in strict mode
var_dump( array_search(0, array("this")),TRUE );

//matching string "this" in array(0), result:true in loose type check
var_dump( array_search("this", array(0)) );
// false in stric mode
var_dump( array_search("this", array(0), TRUE) );

//checking for type FALSE in multidimensional array with loose checking, result:false in loose type check
var_dump( array_search(FALSE,
                   array("a"=> TRUE, "b"=> TRUE,
                         array("c"=> TRUE, "d"=>TRUE)
                        )
                  )
        );

//matching string having integer in beginning, result:true in loose type check
var_dump( array_search('123abc', array(123)) );
var_dump( array_search('123abc', array(123), TRUE) ); // false in strict mode

echo "Done\n";
?>
--EXPECT--
*** Testing resource type with array_search() ***
int(0)
bool(false)

*** Testing miscelleneos inputs with array_search() ***
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
