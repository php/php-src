--TEST--
Test pathinfo() function: usage variations
--FILE--
<?php
echo "*** Testing pathinfo() with miscellaneous input arguments ***\n";

class object_temp {
  public $url_var = "www.foo.com";
  var $html_var = "/var/html/testdir/example.html";
  var $dir_var = "/testdir/foo/test/";
  public $file_var = "/foo//symlink.link";
  var $number = 12345;
}
$obj = new object_temp();

$path_arr = array (
  "www.example.com",
  "/testdir/foo//test/",
  "../foo/test.link",
  "./test/work/scratch/mydir/yourdir/ourdir/test1/test2/test3/test4/test5/test6/test.tmp",
  2.345
);

$paths = array (
  /* pathname containing numeric string */
  0,
  1234,
  -1234,
  2.3456,

  /* pathname as boolean */
  TRUE,
  FALSE,

  /* pathname as an array */
  "./array(1, 2)",
  "array( array(), null)",

  /* pathname as spaces */
  " ",
  ' ',

  /* empty pathname */
  "",
  '',

  /* pathname as members of object */
  $obj->url_var,
  $obj->html_var,
  $obj->dir_var,
  $obj->file_var,
  $obj->number,

  /* pathname as member of array */
  $path_arr[0],
  $path_arr[1],
  $path_arr[2],
  $path_arr[3],
  $path_arr[4]
);

$counter = 1;
/* loop through $paths to test each $path in the above array */
foreach($paths as $path) {
  echo "-- Iteration $counter --\n";
  var_dump( pathinfo($path) );
  var_dump( pathinfo($path, PATHINFO_DIRNAME) );
  var_dump( pathinfo($path, PATHINFO_BASENAME) );
  var_dump( pathinfo($path, PATHINFO_EXTENSION) );
  var_dump( pathinfo($path, PATHINFO_FILENAME) );
  $counter++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing pathinfo() with miscellaneous input arguments ***
-- Iteration 1 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(1) "0"
  ["filename"]=>
  string(1) "0"
}
string(1) "."
string(1) "0"
string(0) ""
string(1) "0"
-- Iteration 2 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(4) "1234"
  ["filename"]=>
  string(4) "1234"
}
string(1) "."
string(4) "1234"
string(0) ""
string(4) "1234"
-- Iteration 3 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(5) "-1234"
  ["filename"]=>
  string(5) "-1234"
}
string(1) "."
string(5) "-1234"
string(0) ""
string(5) "-1234"
-- Iteration 4 --
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(6) "2.3456"
  ["extension"]=>
  string(4) "3456"
  ["filename"]=>
  string(1) "2"
}
string(1) "."
string(6) "2.3456"
string(4) "3456"
string(1) "2"
-- Iteration 5 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(1) "1"
  ["filename"]=>
  string(1) "1"
}
string(1) "."
string(1) "1"
string(0) ""
string(1) "1"
-- Iteration 6 --
array(2) {
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 7 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(11) "array(1, 2)"
  ["filename"]=>
  string(11) "array(1, 2)"
}
string(1) "."
string(11) "array(1, 2)"
string(0) ""
string(11) "array(1, 2)"
-- Iteration 8 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(21) "array( array(), null)"
  ["filename"]=>
  string(21) "array( array(), null)"
}
string(1) "."
string(21) "array( array(), null)"
string(0) ""
string(21) "array( array(), null)"
-- Iteration 9 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(1) " "
  ["filename"]=>
  string(1) " "
}
string(1) "."
string(1) " "
string(0) ""
string(1) " "
-- Iteration 10 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(1) " "
  ["filename"]=>
  string(1) " "
}
string(1) "."
string(1) " "
string(0) ""
string(1) " "
-- Iteration 11 --
array(2) {
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 12 --
array(2) {
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 13 --
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(11) "www.foo.com"
  ["extension"]=>
  string(3) "com"
  ["filename"]=>
  string(7) "www.foo"
}
string(1) "."
string(11) "www.foo.com"
string(3) "com"
string(7) "www.foo"
-- Iteration 14 --
array(4) {
  ["dirname"]=>
  string(17) "/var/html/testdir"
  ["basename"]=>
  string(12) "example.html"
  ["extension"]=>
  string(4) "html"
  ["filename"]=>
  string(7) "example"
}
string(17) "/var/html/testdir"
string(12) "example.html"
string(4) "html"
string(7) "example"
-- Iteration 15 --
array(3) {
  ["dirname"]=>
  string(12) "/testdir/foo"
  ["basename"]=>
  string(4) "test"
  ["filename"]=>
  string(4) "test"
}
string(12) "/testdir/foo"
string(4) "test"
string(0) ""
string(4) "test"
-- Iteration 16 --
array(4) {
  ["dirname"]=>
  string(4) "/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
string(4) "/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
-- Iteration 17 --
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(5) "12345"
  ["filename"]=>
  string(5) "12345"
}
string(1) "."
string(5) "12345"
string(0) ""
string(5) "12345"
-- Iteration 18 --
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(15) "www.example.com"
  ["extension"]=>
  string(3) "com"
  ["filename"]=>
  string(11) "www.example"
}
string(1) "."
string(15) "www.example.com"
string(3) "com"
string(11) "www.example"
-- Iteration 19 --
array(3) {
  ["dirname"]=>
  string(12) "/testdir/foo"
  ["basename"]=>
  string(4) "test"
  ["filename"]=>
  string(4) "test"
}
string(12) "/testdir/foo"
string(4) "test"
string(0) ""
string(4) "test"
-- Iteration 20 --
array(4) {
  ["dirname"]=>
  string(6) "../foo"
  ["basename"]=>
  string(9) "test.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(4) "test"
}
string(6) "../foo"
string(9) "test.link"
string(4) "link"
string(4) "test"
-- Iteration 21 --
array(4) {
  ["dirname"]=>
  string(76) "./test/work/scratch/mydir/yourdir/ourdir/test1/test2/test3/test4/test5/test6"
  ["basename"]=>
  string(8) "test.tmp"
  ["extension"]=>
  string(3) "tmp"
  ["filename"]=>
  string(4) "test"
}
string(76) "./test/work/scratch/mydir/yourdir/ourdir/test1/test2/test3/test4/test5/test6"
string(8) "test.tmp"
string(3) "tmp"
string(4) "test"
-- Iteration 22 --
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(5) "2.345"
  ["extension"]=>
  string(3) "345"
  ["filename"]=>
  string(1) "2"
}
string(1) "."
string(5) "2.345"
string(3) "345"
string(1) "2"
Done
