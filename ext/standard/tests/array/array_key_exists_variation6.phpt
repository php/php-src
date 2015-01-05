--TEST--
Test array_key_exists() function : usage variations -  equality test for certain data types
--FILE--
<?php
/* Prototype  : bool array_key_exists(mixed $key, array $search)
 * Description: Checks if the given key or index exists in the array 
 * Source code: ext/standard/array.c
 * Alias to functions: key_exists
 */

/*
 * Pass certain data types that can be taken as a key in an array 
 * and test whether array_key_exists(() thinks they are equal and therefore
 * returns true when searching for them
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

$unset = 10;
unset($unset);
$array = array ('null' => null, 
                'NULL' => NULL, 
                'empty single quoted string' => '', 
                "empty double quoted string" => "", 
                'undefined variable' => @$undefined,
                'unset variable' => @$unset);

//iterate through original array
foreach($array as $name => $input) {
	$iterator = 1;
	echo "\n-- Key in \$search array is : $name --\n";
	$search[$input] = 'test';
	
	//iterate through array again to see which values are considered equal
	foreach($array as $key) {
		echo "Iteration $iterator:  ";
		var_dump(array_key_exists($key, $search));
		$iterator++;
	}
	$search = null;
}

echo "Done";
?>

--EXPECTF--
*** Testing array_key_exists() : usage variations ***

-- Key in $search array is : null --
Iteration 1:  bool(true)
Iteration 2:  bool(true)
Iteration 3:  bool(true)
Iteration 4:  bool(true)
Iteration 5:  bool(true)
Iteration 6:  bool(true)

-- Key in $search array is : NULL --
Iteration 1:  bool(true)
Iteration 2:  bool(true)
Iteration 3:  bool(true)
Iteration 4:  bool(true)
Iteration 5:  bool(true)
Iteration 6:  bool(true)

-- Key in $search array is : empty single quoted string --
Iteration 1:  bool(true)
Iteration 2:  bool(true)
Iteration 3:  bool(true)
Iteration 4:  bool(true)
Iteration 5:  bool(true)
Iteration 6:  bool(true)

-- Key in $search array is : empty double quoted string --
Iteration 1:  bool(true)
Iteration 2:  bool(true)
Iteration 3:  bool(true)
Iteration 4:  bool(true)
Iteration 5:  bool(true)
Iteration 6:  bool(true)

-- Key in $search array is : undefined variable --
Iteration 1:  bool(true)
Iteration 2:  bool(true)
Iteration 3:  bool(true)
Iteration 4:  bool(true)
Iteration 5:  bool(true)
Iteration 6:  bool(true)

-- Key in $search array is : unset variable --
Iteration 1:  bool(true)
Iteration 2:  bool(true)
Iteration 3:  bool(true)
Iteration 4:  bool(true)
Iteration 5:  bool(true)
Iteration 6:  bool(true)
Done