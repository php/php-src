--TEST--
Directly modifying an unreferenced array when foreach'ing over it.
--FILE--
<?php

define('MAX_LOOPS',5);

function withRefValue($elements, $transform) {
	echo "\n---( Array with $elements element(s): )---\n";
	//Build array:
	for ($i=0; $i<$elements; $i++) {
		$a[] = "v.$i";
	}
	$counter=0;
	
	echo "--> State of array before loop:\n";
	var_dump($a);
	
	echo "--> Do loop:\n";	
	foreach ($a as $k=>$v) {
		echo "     iteration $counter:  \$k=$k; \$v=$v\n";
		eval($transform);
		$counter++;
		if ($counter>MAX_LOOPS) {
			echo "  ** Stuck in a loop! **\n";
			break;
		}
	}
	
	echo "--> State of array after loop:\n";
	var_dump($a);
}


echo "\nPopping elements off end of an unreferenced array";
$transform = 'array_pop($a);';
withRefValue(1, $transform);
withRefValue(2, $transform);
withRefValue(3, $transform);
withRefValue(4, $transform);

echo "\n\n\nShift elements off start of an unreferenced array";
$transform = 'array_shift($a);';
withRefValue(1, $transform);
withRefValue(2, $transform);
withRefValue(3, $transform);
withRefValue(4, $transform);

echo "\n\n\nRemove current element of an unreferenced array";
$transform = 'unset($a[$k]);';
withRefValue(1, $transform);
withRefValue(2, $transform);
withRefValue(3, $transform);
withRefValue(4, $transform);

echo "\n\n\nAdding elements to the end of an unreferenced array";
$transform = 'array_push($a, "new.$counter");';
withRefValue(1, $transform);
withRefValue(2, $transform);
withRefValue(3, $transform);
withRefValue(4, $transform);

echo "\n\n\nAdding elements to the start of an unreferenced array";
$transform = 'array_unshift($a, "new.$counter");';
withRefValue(1, $transform);
withRefValue(2, $transform);
withRefValue(3, $transform);
withRefValue(4, $transform);

?>
--EXPECTF--

Popping elements off end of an unreferenced array
---( Array with 1 element(s): )---
--> State of array before loop:
array(1) {
  [0]=>
  string(3) "v.0"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
--> State of array after loop:
array(0) {
}

---( Array with 2 element(s): )---
--> State of array before loop:
array(2) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
--> State of array after loop:
array(0) {
}

---( Array with 3 element(s): )---
--> State of array before loop:
array(3) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
--> State of array after loop:
array(0) {
}

---( Array with 4 element(s): )---
--> State of array before loop:
array(4) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
  [3]=>
  string(3) "v.3"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
     iteration 3:  $k=3; $v=v.3
--> State of array after loop:
array(0) {
}



Shift elements off start of an unreferenced array
---( Array with 1 element(s): )---
--> State of array before loop:
array(1) {
  [0]=>
  string(3) "v.0"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
--> State of array after loop:
array(0) {
}

---( Array with 2 element(s): )---
--> State of array before loop:
array(2) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
--> State of array after loop:
array(0) {
}

---( Array with 3 element(s): )---
--> State of array before loop:
array(3) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
--> State of array after loop:
array(0) {
}

---( Array with 4 element(s): )---
--> State of array before loop:
array(4) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
  [3]=>
  string(3) "v.3"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
     iteration 3:  $k=3; $v=v.3
--> State of array after loop:
array(0) {
}



Remove current element of an unreferenced array
---( Array with 1 element(s): )---
--> State of array before loop:
array(1) {
  [0]=>
  string(3) "v.0"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
--> State of array after loop:
array(0) {
}

---( Array with 2 element(s): )---
--> State of array before loop:
array(2) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
--> State of array after loop:
array(0) {
}

---( Array with 3 element(s): )---
--> State of array before loop:
array(3) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
--> State of array after loop:
array(0) {
}

---( Array with 4 element(s): )---
--> State of array before loop:
array(4) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
  [3]=>
  string(3) "v.3"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
     iteration 3:  $k=3; $v=v.3
--> State of array after loop:
array(0) {
}



Adding elements to the end of an unreferenced array
---( Array with 1 element(s): )---
--> State of array before loop:
array(1) {
  [0]=>
  string(3) "v.0"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
--> State of array after loop:
array(2) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(5) "new.0"
}

---( Array with 2 element(s): )---
--> State of array before loop:
array(2) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
--> State of array after loop:
array(4) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(5) "new.0"
  [3]=>
  string(5) "new.1"
}

---( Array with 3 element(s): )---
--> State of array before loop:
array(3) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
--> State of array after loop:
array(6) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
  [3]=>
  string(5) "new.0"
  [4]=>
  string(5) "new.1"
  [5]=>
  string(5) "new.2"
}

---( Array with 4 element(s): )---
--> State of array before loop:
array(4) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
  [3]=>
  string(3) "v.3"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
     iteration 3:  $k=3; $v=v.3
--> State of array after loop:
array(8) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
  [3]=>
  string(3) "v.3"
  [4]=>
  string(5) "new.0"
  [5]=>
  string(5) "new.1"
  [6]=>
  string(5) "new.2"
  [7]=>
  string(5) "new.3"
}



Adding elements to the start of an unreferenced array
---( Array with 1 element(s): )---
--> State of array before loop:
array(1) {
  [0]=>
  string(3) "v.0"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
--> State of array after loop:
array(2) {
  [0]=>
  string(5) "new.0"
  [1]=>
  string(3) "v.0"
}

---( Array with 2 element(s): )---
--> State of array before loop:
array(2) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
--> State of array after loop:
array(4) {
  [0]=>
  string(5) "new.1"
  [1]=>
  string(5) "new.0"
  [2]=>
  string(3) "v.0"
  [3]=>
  string(3) "v.1"
}

---( Array with 3 element(s): )---
--> State of array before loop:
array(3) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
--> State of array after loop:
array(6) {
  [0]=>
  string(5) "new.2"
  [1]=>
  string(5) "new.1"
  [2]=>
  string(5) "new.0"
  [3]=>
  string(3) "v.0"
  [4]=>
  string(3) "v.1"
  [5]=>
  string(3) "v.2"
}

---( Array with 4 element(s): )---
--> State of array before loop:
array(4) {
  [0]=>
  string(3) "v.0"
  [1]=>
  string(3) "v.1"
  [2]=>
  string(3) "v.2"
  [3]=>
  string(3) "v.3"
}
--> Do loop:
     iteration 0:  $k=0; $v=v.0
     iteration 1:  $k=1; $v=v.1
     iteration 2:  $k=2; $v=v.2
     iteration 3:  $k=3; $v=v.3
--> State of array after loop:
array(8) {
  [0]=>
  string(5) "new.3"
  [1]=>
  string(5) "new.2"
  [2]=>
  string(5) "new.1"
  [3]=>
  string(5) "new.0"
  [4]=>
  string(3) "v.0"
  [5]=>
  string(3) "v.1"
  [6]=>
  string(3) "v.2"
  [7]=>
  string(3) "v.3"
}
