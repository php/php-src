--TEST--
Test array_splice() function : usage variations - lengths and offsets
--FILE--
<?php
/* 
 * proto array array_splice(array input, int offset [, int length [, array replacement]])
 * Function is implemented in ext/standard/array.c
*/ 

echo "*** array_splice() function : usage variations - lengths and offsets\n";


function test_splice ($offset, $length)
{
	echo "  - No replacement\n";
	$input_array=array(0,1,2,3,4,5);
	var_dump (array_splice ($input_array,$offset,$length));
	var_dump ($input_array);
    echo "  - With replacement\n";
    $input_array=array(0,1,2,3,4,5);
    var_dump (array_splice ($input_array,$offset,$length,array ("A","B","C")));
	var_dump ($input_array);
}

echo "absolute offset - absolute length - cut from beginning\n";
test_splice (0,2);
echo "absolute offset - absolute length - cut from middle\n";
test_splice (2,2);
echo "absolute offset - absolute length - cut from end\n";
test_splice (4,2);
echo "absolute offset - absolute length - attempt to cut past end\n";
test_splice (4,4);
echo "absolute offset - absolute length - cut everything\n";
test_splice (0,7);
echo "absolute offset - absolute length - cut nothing\n";
test_splice (3,0);

echo "absolute offset - relative length - cut from beginning\n";
test_splice (0,-4);

echo "absolute offset - relative length - cut from middle\n";
test_splice (2,-2);

echo "absolute offset - relative length - attempt to cut form before beginning \n";
test_splice (0,-7);

echo "absolute offset - relative length - cut nothing\n";
test_splice (2,-7);

echo "relative offset - absolute length - cut from beginning\n";
test_splice (-6,2);

echo "relative offset - absolute length - cut from middle\n";
test_splice (-4,2);
echo "relative offset - absolute length - cut from end\n";
test_splice (-2,2);
echo "relative offset - absolute length - attempt to cut past end\n";
test_splice (-2,4);
echo "relative offset - absolute length - cut everything\n";
test_splice (-6,6);
echo "relative offset - absolute length - cut nothing\n";
test_splice (-6,0);

echo "relative offset - relative length - cut from beginning\n";
test_splice (-6,-4);

echo "relative offset - relative length - cut from middle\n";
test_splice (-4,-2);

echo "relative offset - relative length - cut nothing\n";
test_splice (-4,-7);
echo "Done\n";
?>

--EXPECT--
*** array_splice() function : usage variations - lengths and offsets
absolute offset - absolute length - cut from beginning
  - No replacement
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
  - With replacement
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(7) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  int(2)
  [4]=>
  int(3)
  [5]=>
  int(4)
  [6]=>
  int(5)
}
absolute offset - absolute length - cut from middle
  - No replacement
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
  - With replacement
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  string(1) "A"
  [3]=>
  string(1) "B"
  [4]=>
  string(1) "C"
  [5]=>
  int(4)
  [6]=>
  int(5)
}
absolute offset - absolute length - cut from end
  - No replacement
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
  - With replacement
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  string(1) "A"
  [5]=>
  string(1) "B"
  [6]=>
  string(1) "C"
}
absolute offset - absolute length - attempt to cut past end
  - No replacement
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
  - With replacement
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  string(1) "A"
  [5]=>
  string(1) "B"
  [6]=>
  string(1) "C"
}
absolute offset - absolute length - cut everything
  - No replacement
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
array(0) {
}
  - With replacement
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
array(3) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
}
absolute offset - absolute length - cut nothing
  - No replacement
array(0) {
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
  - With replacement
array(0) {
}
array(9) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  string(1) "A"
  [4]=>
  string(1) "B"
  [5]=>
  string(1) "C"
  [6]=>
  int(3)
  [7]=>
  int(4)
  [8]=>
  int(5)
}
absolute offset - relative length - cut from beginning
  - No replacement
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
  - With replacement
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(7) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  int(2)
  [4]=>
  int(3)
  [5]=>
  int(4)
  [6]=>
  int(5)
}
absolute offset - relative length - cut from middle
  - No replacement
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
  - With replacement
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  string(1) "A"
  [3]=>
  string(1) "B"
  [4]=>
  string(1) "C"
  [5]=>
  int(4)
  [6]=>
  int(5)
}
absolute offset - relative length - attempt to cut form before beginning 
  - No replacement
array(0) {
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
  - With replacement
array(0) {
}
array(9) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  int(0)
  [4]=>
  int(1)
  [5]=>
  int(2)
  [6]=>
  int(3)
  [7]=>
  int(4)
  [8]=>
  int(5)
}
absolute offset - relative length - cut nothing
  - No replacement
array(0) {
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
  - With replacement
array(0) {
}
array(9) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  string(1) "A"
  [3]=>
  string(1) "B"
  [4]=>
  string(1) "C"
  [5]=>
  int(2)
  [6]=>
  int(3)
  [7]=>
  int(4)
  [8]=>
  int(5)
}
relative offset - absolute length - cut from beginning
  - No replacement
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
  - With replacement
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(7) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  int(2)
  [4]=>
  int(3)
  [5]=>
  int(4)
  [6]=>
  int(5)
}
relative offset - absolute length - cut from middle
  - No replacement
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
  - With replacement
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  string(1) "A"
  [3]=>
  string(1) "B"
  [4]=>
  string(1) "C"
  [5]=>
  int(4)
  [6]=>
  int(5)
}
relative offset - absolute length - cut from end
  - No replacement
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
  - With replacement
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  string(1) "A"
  [5]=>
  string(1) "B"
  [6]=>
  string(1) "C"
}
relative offset - absolute length - attempt to cut past end
  - No replacement
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
  - With replacement
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(5)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  string(1) "A"
  [5]=>
  string(1) "B"
  [6]=>
  string(1) "C"
}
relative offset - absolute length - cut everything
  - No replacement
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
array(0) {
}
  - With replacement
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
array(3) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
}
relative offset - absolute length - cut nothing
  - No replacement
array(0) {
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
  - With replacement
array(0) {
}
array(9) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  int(0)
  [4]=>
  int(1)
  [5]=>
  int(2)
  [6]=>
  int(3)
  [7]=>
  int(4)
  [8]=>
  int(5)
}
relative offset - relative length - cut from beginning
  - No replacement
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
  - With replacement
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(7) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  int(2)
  [4]=>
  int(3)
  [5]=>
  int(4)
  [6]=>
  int(5)
}
relative offset - relative length - cut from middle
  - No replacement
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
  - With replacement
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(7) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  string(1) "A"
  [3]=>
  string(1) "B"
  [4]=>
  string(1) "C"
  [5]=>
  int(4)
  [6]=>
  int(5)
}
relative offset - relative length - cut nothing
  - No replacement
array(0) {
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
  - With replacement
array(0) {
}
array(9) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  string(1) "A"
  [3]=>
  string(1) "B"
  [4]=>
  string(1) "C"
  [5]=>
  int(2)
  [6]=>
  int(3)
  [7]=>
  int(4)
  [8]=>
  int(5)
}
Done