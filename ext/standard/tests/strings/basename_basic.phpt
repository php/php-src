--TEST--
Test basename() function : basic functionality
--FILE--
<?php
$file_paths = array (
  /* simple paths */
  array("bar"),
  array("/foo/bar"),
  array("foo/bar"),
  array("/bar"),

  /* simple paths with trailing slashes */
  array("bar/"),
  array("/bar/"),
  array("/foo/bar/"),
  array("foo/bar/"),
  array("/bar/"),

  /* paths with suffix removal */
  array("bar.gz", ".gz"),
  array("bar.gz", "bar.gz"),
  array("/foo/bar.gz", ".gz"),
  array("foo/bar.gz", ".gz"),
  array("/bar.gz", ".gz"),

  /* paths with suffix and trailing slashes with suffix removal*/
  array("bar.gz/", ".gz"),
  array("/bar.gz/", ".gz"),
  array("/foo/bar.gz/", ".gz"),
  array("foo/bar.gz/", ".gz"),
  array("/bar.gz/", ".gz"),

  /* paths with basename only suffix, with suffix removal*/
  array("/.gz", ".gz"),
  array(".gz", ".gz"),
  array("/foo/.gz", ".gz"),

  /* paths with basename only suffix & trailing slashes, with suffix removal*/
  array(".gz/", ".gz"),
  array("/foo/.gz/", ".gz"),
  array("foo/.gz/", ".gz"),

  /* paths with binary value to check if the function is binary safe*/
  array("foo".chr(0)."bar"),
  array("/foo".chr(0)."bar"),
  array("/foo".chr(0)."bar/"),
  array("foo".chr(0)."bar/"),
  array("foo".chr(0)."bar/test"),
  array("/foo".chr(0)."bar/bar.gz", ".gz"),
  array("/foo".chr(0)."bar/bar.gz")
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
string(6) "bar.gz"

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
string(3) ".gz"

--Iteration 21--
string(3) ".gz"

--Iteration 22--
string(3) ".gz"

--Iteration 23--
string(3) ".gz"

--Iteration 24--
string(3) ".gz"

--Iteration 25--
string(3) ".gz"

--Iteration 26--
string(7) "foo%0bar"

--Iteration 27--
string(7) "foo%0bar"

--Iteration 28--
string(7) "foo%0bar"

--Iteration 29--
string(7) "foo%0bar"

--Iteration 30--
string(4) "test"

--Iteration 31--
string(3) "bar"

--Iteration 32--
string(6) "bar.gz"
Done
