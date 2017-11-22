--TEST--
Foreach loop tests - foreach operates on the original array if the array is referenced outside the loop.
--FILE--
<?php
// From php.net/foreach:
// "Unless the array is referenced, foreach operates on a copy of the specified array."

echo "\nRemove elements from a referenced array during loop\n";
$refedArray=array("original.0", "original.1", "original.2");
$ref=&$refedArray;
foreach ($refedArray as $k=>$v1) {
	array_pop($refedArray);
	echo "key: $k; value: $v1\n";
}

echo "\nRemove elements from a referenced array during loop, using &\$value\n";
$refedArray=array("original.0", "original.1", "original.2");
$ref=&$refedArray;
foreach ($refedArray as $k=>&$v2) {
	array_pop($refedArray);
	echo "key: $k; value: $v2\n";
}

echo "\nAdd elements to a referenced array during loop\n";
$refedArray=array("original.0", "original.1", "original.2");
$ref=&$refedArray;
$count=0;
foreach ($refedArray as $k=>$v3) {
	array_push($refedArray, "new.$k");
	echo "key: $k; value: $v3\n";
	
	if ($count++>5) {
		echo "Loop detected, as expected.\n";
		break;
	}
}

echo "\nAdd elements to a referenced array during loop, using &\$value\n";
$refedArray=array("original.0", "original.1", "original.2");
$ref=&$refedArray;
$count=0;
foreach ($refedArray as $k=>&$v4) {
	array_push($refedArray, "new.$k");
	echo "key: $k; value: $v4\n";
	
	if ($count++>5) {
		echo "Loop detected, as expected.\n";
		break;
	}
}

?>
--EXPECT--

Remove elements from a referenced array during loop
key: 0; value: original.0
key: 1; value: original.1
key: 2; value: original.2

Remove elements from a referenced array during loop, using &$value
key: 0; value: original.0
key: 1; value: original.1

Add elements to a referenced array during loop
key: 0; value: original.0
key: 1; value: original.1
key: 2; value: original.2

Add elements to a referenced array during loop, using &$value
key: 0; value: original.0
key: 1; value: original.1
key: 2; value: original.2
key: 3; value: new.0
key: 4; value: new.1
key: 5; value: new.2
key: 6; value: new.3
Loop detected, as expected.

