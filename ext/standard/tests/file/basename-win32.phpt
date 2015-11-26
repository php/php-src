--TEST--
basename
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip Windows only basename tests');
}
?>
--FILE--
<?php
/* 
 * proto string basename(string path [, string suffix])
 * Function is implemented in ext/standard/string.c
 */ 
$file_paths = array (
  /* simple paths */
  array("bar"),
  array("\\foo\\bar"),
  array("foo\\bar"),
  array("\\bar"),

  /* simple paths with trailing slashes */
  array("bar\\"),
  array("\\bar\\"),
  array("\\foo\\bar\\"),
  array("foo\\bar\\"),
  array("\\bar\\"),

  /* paths with suffix removal */
  array("bar.zip", ".zip"),
  array("bar.zip", "bar.zip"),
  array("\\foo\\bar.zip", ".zip"),
  array("foo\\bar.zip", ".zip"),
  array("\\bar.zip", ".zip"),  

  /* paths with suffix and trailing slashes with suffix removal*/
  array("bar.zip\\", ".zip"),
  array("\\bar.zip\\", ".zip"),
  array("\\foo\\bar.zip\\", ".zip"),
  array("foo\\bar.zip\\", ".zip"),
  array("\\bar.zip\\", ".zip"),  
  
  /* paths with basename only suffix, with suffix removal*/
  array("\\.zip", ".zip"),
  array(".zip", ".zip"),
  array("\\foo\\.zip", ".zip"),

  /* paths with basename only suffix & trailing slashes, with suffix removal*/
  array(".zip\\", ".zip"),
  array("\\foo\\.zip\\", ".zip"),
  array("foo\\.zip\\", ".zip"),
);

$file_path_variations = array (
  /* paths with shortcut home dir char, with suffix variation */
  array("C:\\temp\\bar"),
  array("C:\\temp\\bar", ""),
  array("C:\\temp\\bar", NULL),
  array("C:\\temp\\bar", ' '),
  array("C:\\temp\\bar.tar", ".tar"),
  array("C:\\temp\\bar.tar", "~"),
  array("C:\\temp\\bar.tar\\", "~"),
  array("C:\\temp\\bar.tar\\", ""),
  array("C:\\temp\\bar.tar", NULL),
  array("C:\\temp\\bar.tar", ''),
  array("C:\\temp\\bar.tar", " "),

  /* paths with numeric strings */
  array("10.5"),
  array("10.5", ".5"),
  array("10.5", "10.5"),
  array("10"),
  array("105", "5"),
  array("/10.5"),
  array("10.5\\"),
  array("10/10.zip"),
  array("0"),
  array('0'),

  /* paths and suffix given as same */
  array("bar.zip", "bar.zip"),
  array("\\bar.zip", "\\bar.zip"),
  array("\\bar.zip\\", "\\bar.zip\\"),
  array(" ", " "),
  array(' ', ' '),
  array(NULL, NULL),

  /* path with spaces */
  array(" "),
  array(' '),
  
  /* empty paths */
  array(""),
  array(''),
  array(NULL)
);

function check_basename( $path_arrays ) {
   $loop_counter = 1;
   foreach ($path_arrays as $path) {
     echo "\n--Iteration $loop_counter--\n"; $loop_counter++;
     if( 1 == count($path) ) { // no suffix provided
       var_dump( basename($path[0]) );
     } else { // path as well as suffix provided,
       var_dump( basename($path[0], $path[1]) );    
     } 
   }
}

echo "*** Testing basic operations ***\n";
check_basename( $file_paths );

echo "\n*** Testing possible variations in path and suffix ***\n";
check_basename( $file_path_variations );

echo "\n*** Testing error conditions ***\n";
// zero arguments 
var_dump( basename() );

// more than expected no. of arguments
var_dump( basename("\\blah\\tmp\\bar.zip", ".zip", ".zip") );

// passing invalid type arguments 
$object = new stdclass;
var_dump( basename( array("string\\bar") ) );
var_dump( basename( array("string\\bar"), "bar" ) );
var_dump( basename( "bar", array("string\\bar") ) );
var_dump( basename( $object, "bar" ) );
var_dump( basename( $object ) );
var_dump( basename( $object, $object ) );
var_dump( basename( "bar", $object ) );

echo "Done\n";
?>
--EXPECTF--
*** Testing basic operations ***

--Iteration 1--
string(3) "bar"

--Iteration 2--
string(3) "bar"

--Iteration 3--
string(3) "bar"

--Iteration 4--
string(3) "bar"

--Iteration 5--
string(3) "bar"

--Iteration 6--
string(3) "bar"

--Iteration 7--
string(3) "bar"

--Iteration 8--
string(3) "bar"

--Iteration 9--
string(3) "bar"

--Iteration 10--
string(3) "bar"

--Iteration 11--
string(7) "bar.zip"

--Iteration 12--
string(3) "bar"

--Iteration 13--
string(3) "bar"

--Iteration 14--
string(3) "bar"

--Iteration 15--
string(3) "bar"

--Iteration 16--
string(3) "bar"

--Iteration 17--
string(3) "bar"

--Iteration 18--
string(3) "bar"

--Iteration 19--
string(3) "bar"

--Iteration 20--
string(4) ".zip"

--Iteration 21--
string(4) ".zip"

--Iteration 22--
string(4) ".zip"

--Iteration 23--
string(4) ".zip"

--Iteration 24--
string(4) ".zip"

--Iteration 25--
string(4) ".zip"

*** Testing possible variations in path and suffix ***

--Iteration 1--
string(3) "bar"

--Iteration 2--
string(3) "bar"

--Iteration 3--
string(3) "bar"

--Iteration 4--
string(3) "bar"

--Iteration 5--
string(3) "bar"

--Iteration 6--
string(7) "bar.tar"

--Iteration 7--
string(7) "bar.tar"

--Iteration 8--
string(7) "bar.tar"

--Iteration 9--
string(7) "bar.tar"

--Iteration 10--
string(7) "bar.tar"

--Iteration 11--
string(7) "bar.tar"

--Iteration 12--
string(4) "10.5"

--Iteration 13--
string(2) "10"

--Iteration 14--
string(4) "10.5"

--Iteration 15--
string(2) "10"

--Iteration 16--
string(2) "10"

--Iteration 17--
string(4) "10.5"

--Iteration 18--
string(4) "10.5"

--Iteration 19--
string(6) "10.zip"

--Iteration 20--
string(1) "0"

--Iteration 21--
string(1) "0"

--Iteration 22--
string(7) "bar.zip"

--Iteration 23--
string(7) "bar.zip"

--Iteration 24--
string(7) "bar.zip"

--Iteration 25--
string(1) " "

--Iteration 26--
string(1) " "

--Iteration 27--
string(0) ""

--Iteration 28--
string(1) " "

--Iteration 29--
string(1) " "

--Iteration 30--
string(0) ""

--Iteration 31--
string(0) ""

--Iteration 32--
string(0) ""

*** Testing error conditions ***

Warning: basename() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: basename() expects at most 2 parameters, 3 given in %s on line %d
NULL

Warning: basename() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: basename() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: basename() expects parameter 2 to be string, array given in %s on line %d
NULL

Warning: basename() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: basename() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: basename() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: basename() expects parameter 2 to be string, object given in %s on line %d
NULL
Done
