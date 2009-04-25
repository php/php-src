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
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(1) "0"
  [%u|b%"filename"]=>
  %unicode|string%(1) "0"
}
%unicode|string%(1) "."
%unicode|string%(1) "0"
%unicode|string%(0) ""
%unicode|string%(1) "0"
-- Iteration 2 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(4) "1234"
  [%u|b%"filename"]=>
  %unicode|string%(4) "1234"
}
%unicode|string%(1) "."
%unicode|string%(4) "1234"
%unicode|string%(0) ""
%unicode|string%(4) "1234"
-- Iteration 3 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(5) "-1234"
  [%u|b%"filename"]=>
  %unicode|string%(5) "-1234"
}
%unicode|string%(1) "."
%unicode|string%(5) "-1234"
%unicode|string%(0) ""
%unicode|string%(5) "-1234"
-- Iteration 4 --
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(6) "2.3456"
  [%u|b%"extension"]=>
  %unicode|string%(4) "3456"
  [%u|b%"filename"]=>
  %unicode|string%(1) "2"
}
%unicode|string%(1) "."
%unicode|string%(6) "2.3456"
%unicode|string%(4) "3456"
%unicode|string%(1) "2"
-- Iteration 5 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(1) "1"
  [%u|b%"filename"]=>
  %unicode|string%(1) "1"
}
%unicode|string%(1) "."
%unicode|string%(1) "1"
%unicode|string%(0) ""
%unicode|string%(1) "1"
-- Iteration 6 --
array(2) {
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
-- Iteration 7 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(11) "array(1, 2)"
  [%u|b%"filename"]=>
  %unicode|string%(11) "array(1, 2)"
}
%unicode|string%(1) "."
%unicode|string%(11) "array(1, 2)"
%unicode|string%(0) ""
%unicode|string%(11) "array(1, 2)"
-- Iteration 8 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(21) "array( array(), null)"
  [%u|b%"filename"]=>
  %unicode|string%(21) "array( array(), null)"
}
%unicode|string%(1) "."
%unicode|string%(21) "array( array(), null)"
%unicode|string%(0) ""
%unicode|string%(21) "array( array(), null)"
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
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(1) " "
  [%u|b%"filename"]=>
  %unicode|string%(1) " "
}
%unicode|string%(1) "."
%unicode|string%(1) " "
%unicode|string%(0) ""
%unicode|string%(1) " "
-- Iteration 11 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(1) " "
  [%u|b%"filename"]=>
  %unicode|string%(1) " "
}
%unicode|string%(1) "."
%unicode|string%(1) " "
%unicode|string%(0) ""
%unicode|string%(1) " "
-- Iteration 12 --
array(2) {
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
-- Iteration 13 --
array(2) {
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
-- Iteration 14 --
array(2) {
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
-- Iteration 15 --
array(2) {
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
-- Iteration 16 --
array(2) {
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
-- Iteration 17 --
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(11) "www.foo.com"
  [%u|b%"extension"]=>
  %unicode|string%(3) "com"
  [%u|b%"filename"]=>
  %unicode|string%(7) "www.foo"
}
%unicode|string%(1) "."
%unicode|string%(11) "www.foo.com"
%unicode|string%(3) "com"
%unicode|string%(7) "www.foo"
-- Iteration 18 --
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(17) "/var/html/testdir"
  [%u|b%"basename"]=>
  %unicode|string%(12) "example.html"
  [%u|b%"extension"]=>
  %unicode|string%(4) "html"
  [%u|b%"filename"]=>
  %unicode|string%(7) "example"
}
%unicode|string%(17) "/var/html/testdir"
%unicode|string%(12) "example.html"
%unicode|string%(4) "html"
%unicode|string%(7) "example"
-- Iteration 19 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(12) "/testdir/foo"
  [%u|b%"basename"]=>
  %unicode|string%(4) "test"
  [%u|b%"filename"]=>
  %unicode|string%(4) "test"
}
%unicode|string%(12) "/testdir/foo"
%unicode|string%(4) "test"
%unicode|string%(0) ""
%unicode|string%(4) "test"
-- Iteration 20 --
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(4) "/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
%unicode|string%(4) "/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
-- Iteration 21 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(5) "12345"
  [%u|b%"filename"]=>
  %unicode|string%(5) "12345"
}
%unicode|string%(1) "."
%unicode|string%(5) "12345"
%unicode|string%(0) ""
%unicode|string%(5) "12345"
-- Iteration 22 --
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(15) "www.example.com"
  [%u|b%"extension"]=>
  %unicode|string%(3) "com"
  [%u|b%"filename"]=>
  %unicode|string%(11) "www.example"
}
%unicode|string%(1) "."
%unicode|string%(15) "www.example.com"
%unicode|string%(3) "com"
%unicode|string%(11) "www.example"
-- Iteration 23 --
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(12) "/testdir/foo"
  [%u|b%"basename"]=>
  %unicode|string%(4) "test"
  [%u|b%"filename"]=>
  %unicode|string%(4) "test"
}
%unicode|string%(12) "/testdir/foo"
%unicode|string%(4) "test"
%unicode|string%(0) ""
%unicode|string%(4) "test"
-- Iteration 24 --
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(6) "../foo"
  [%u|b%"basename"]=>
  %unicode|string%(9) "test.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(4) "test"
}
%unicode|string%(6) "../foo"
%unicode|string%(9) "test.link"
%unicode|string%(4) "link"
%unicode|string%(4) "test"
-- Iteration 25 --
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(76) "./test/work/scratch/mydir/yourdir/ourdir/test1/test2/test3/test4/test5/test6"
  [%u|b%"basename"]=>
  %unicode|string%(8) "test.tmp"
  [%u|b%"extension"]=>
  %unicode|string%(3) "tmp"
  [%u|b%"filename"]=>
  %unicode|string%(4) "test"
}
%unicode|string%(76) "./test/work/scratch/mydir/yourdir/ourdir/test1/test2/test3/test4/test5/test6"
%unicode|string%(8) "test.tmp"
%unicode|string%(3) "tmp"
%unicode|string%(4) "test"
-- Iteration 26 --
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(5) "2.345"
  [%u|b%"extension"]=>
  %unicode|string%(3) "345"
  [%u|b%"filename"]=>
  %unicode|string%(1) "2"
}
%unicode|string%(1) "."
%unicode|string%(5) "2.345"
%unicode|string%(3) "345"
%unicode|string%(1) "2"
Done
