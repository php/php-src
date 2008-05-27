--TEST--
Test pathinfo() function: usage variations
--FILE--
<?php
/* Prototype: mixed pathinfo ( string $path [, int $options] );
   Description: Returns information about a file path
*/

echo "*** Testing pathinfo() with miscelleneous input arguments ***\n";

$fp = fopen(__FILE__, "r");
unset($fp);

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

  /* pathname as object */
  $obj,

  /* pathname as spaces */
  " ",
  ' ',

  /* empty pathname */
  "",
  '',

  /* pathname as NULL */
  NULL,
  null,
  
  /* filename as resource */
  $fp,

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
--EXPECTF--
*** Testing pathinfo() with miscelleneous input arguments ***

Notice: Undefined variable: fp in %s on line %d
-- Iteration 1 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(1) "0"
  [u"filename"]=>
  unicode(1) "0"
}
unicode(1) "."
unicode(1) "0"
string(0) ""
unicode(1) "0"
-- Iteration 2 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(4) "1234"
  [u"filename"]=>
  unicode(4) "1234"
}
unicode(1) "."
unicode(4) "1234"
string(0) ""
unicode(4) "1234"
-- Iteration 3 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(5) "-1234"
  [u"filename"]=>
  unicode(5) "-1234"
}
unicode(1) "."
unicode(5) "-1234"
string(0) ""
unicode(5) "-1234"
-- Iteration 4 --
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(6) "2.3456"
  [u"extension"]=>
  unicode(4) "3456"
  [u"filename"]=>
  unicode(1) "2"
}
unicode(1) "."
unicode(6) "2.3456"
unicode(4) "3456"
unicode(1) "2"
-- Iteration 5 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(1) "1"
  [u"filename"]=>
  unicode(1) "1"
}
unicode(1) "."
unicode(1) "1"
string(0) ""
unicode(1) "1"
-- Iteration 6 --
array(2) {
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
string(0) ""
unicode(0) ""
string(0) ""
unicode(0) ""
-- Iteration 7 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(11) "array(1, 2)"
  [u"filename"]=>
  unicode(11) "array(1, 2)"
}
unicode(1) "."
unicode(11) "array(1, 2)"
string(0) ""
unicode(11) "array(1, 2)"
-- Iteration 8 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(21) "array( array(), null)"
  [u"filename"]=>
  unicode(21) "array( array(), null)"
}
unicode(1) "."
unicode(21) "array( array(), null)"
string(0) ""
unicode(21) "array( array(), null)"
-- Iteration 9 --

Warning: pathinfo() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL

Warning: pathinfo() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL

Warning: pathinfo() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL

Warning: pathinfo() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL

Warning: pathinfo() expects parameter 1 to be string (Unicode or binary), object given in %s on line %d
NULL
-- Iteration 10 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(1) " "
  [u"filename"]=>
  unicode(1) " "
}
unicode(1) "."
unicode(1) " "
string(0) ""
unicode(1) " "
-- Iteration 11 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(1) " "
  [u"filename"]=>
  unicode(1) " "
}
unicode(1) "."
unicode(1) " "
string(0) ""
unicode(1) " "
-- Iteration 12 --
array(2) {
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
string(0) ""
unicode(0) ""
string(0) ""
unicode(0) ""
-- Iteration 13 --
array(2) {
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
string(0) ""
unicode(0) ""
string(0) ""
unicode(0) ""
-- Iteration 14 --
array(2) {
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
string(0) ""
unicode(0) ""
string(0) ""
unicode(0) ""
-- Iteration 15 --
array(2) {
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
string(0) ""
unicode(0) ""
string(0) ""
unicode(0) ""
-- Iteration 16 --
array(2) {
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
string(0) ""
unicode(0) ""
string(0) ""
unicode(0) ""
-- Iteration 17 --
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(11) "www.foo.com"
  [u"extension"]=>
  unicode(3) "com"
  [u"filename"]=>
  unicode(7) "www.foo"
}
unicode(1) "."
unicode(11) "www.foo.com"
unicode(3) "com"
unicode(7) "www.foo"
-- Iteration 18 --
array(4) {
  [u"dirname"]=>
  unicode(17) "/var/html/testdir"
  [u"basename"]=>
  unicode(12) "example.html"
  [u"extension"]=>
  unicode(4) "html"
  [u"filename"]=>
  unicode(7) "example"
}
unicode(17) "/var/html/testdir"
unicode(12) "example.html"
unicode(4) "html"
unicode(7) "example"
-- Iteration 19 --
array(3) {
  [u"dirname"]=>
  unicode(12) "/testdir/foo"
  [u"basename"]=>
  unicode(4) "test"
  [u"filename"]=>
  unicode(4) "test"
}
unicode(12) "/testdir/foo"
unicode(4) "test"
string(0) ""
unicode(4) "test"
-- Iteration 20 --
array(4) {
  [u"dirname"]=>
  unicode(4) "/foo"
  [u"basename"]=>
  unicode(12) "symlink.link"
  [u"extension"]=>
  unicode(4) "link"
  [u"filename"]=>
  unicode(7) "symlink"
}
unicode(4) "/foo"
unicode(12) "symlink.link"
unicode(4) "link"
unicode(7) "symlink"
-- Iteration 21 --
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(5) "12345"
  [u"filename"]=>
  unicode(5) "12345"
}
unicode(1) "."
unicode(5) "12345"
string(0) ""
unicode(5) "12345"
-- Iteration 22 --
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(15) "www.example.com"
  [u"extension"]=>
  unicode(3) "com"
  [u"filename"]=>
  unicode(11) "www.example"
}
unicode(1) "."
unicode(15) "www.example.com"
unicode(3) "com"
unicode(11) "www.example"
-- Iteration 23 --
array(3) {
  [u"dirname"]=>
  unicode(12) "/testdir/foo"
  [u"basename"]=>
  unicode(4) "test"
  [u"filename"]=>
  unicode(4) "test"
}
unicode(12) "/testdir/foo"
unicode(4) "test"
string(0) ""
unicode(4) "test"
-- Iteration 24 --
array(4) {
  [u"dirname"]=>
  unicode(6) "../foo"
  [u"basename"]=>
  unicode(9) "test.link"
  [u"extension"]=>
  unicode(4) "link"
  [u"filename"]=>
  unicode(4) "test"
}
unicode(6) "../foo"
unicode(9) "test.link"
unicode(4) "link"
unicode(4) "test"
-- Iteration 25 --
array(4) {
  [u"dirname"]=>
  unicode(76) "./test/work/scratch/mydir/yourdir/ourdir/test1/test2/test3/test4/test5/test6"
  [u"basename"]=>
  unicode(8) "test.tmp"
  [u"extension"]=>
  unicode(3) "tmp"
  [u"filename"]=>
  unicode(4) "test"
}
unicode(76) "./test/work/scratch/mydir/yourdir/ourdir/test1/test2/test3/test4/test5/test6"
unicode(8) "test.tmp"
unicode(3) "tmp"
unicode(4) "test"
-- Iteration 26 --
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(5) "2.345"
  [u"extension"]=>
  unicode(3) "345"
  [u"filename"]=>
  unicode(1) "2"
}
unicode(1) "."
unicode(5) "2.345"
unicode(3) "345"
unicode(1) "2"
Done
