--TEST--
Test basename() function : basic functionality
--FILE--
<?php
/* Prototype: string basename ( string $path [, string $suffix] );
   Description: Given a string containing a path to a file,
                this function will return the base name of the file. 
                If the filename ends in suffix this will also be cut off.
*/
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
  array( b"foo/bar" ),
  array( b"/foo/bar" ),
  array( b"/foo/bar/" ),
  array( b"foo/bar" ),
  array( b"foo/bar/" ),
  array( b"/foo/bar/bar.gz", b".gz" ),
  array( b"/foo/bar/test/bar.gz" )
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
--EXPECT--
*** Testing basic operations ***

--Iteration 1--
unicode(3) "bar"

--Iteration 2--
unicode(3) "bar"

--Iteration 3--
unicode(3) "bar"

--Iteration 4--
unicode(3) "bar"

--Iteration 5--
unicode(3) "bar"

--Iteration 6--
unicode(3) "bar"

--Iteration 7--
unicode(3) "bar"

--Iteration 8--
unicode(3) "bar"

--Iteration 9--
unicode(3) "bar"

--Iteration 10--
unicode(3) "bar"

--Iteration 11--
unicode(6) "bar.gz"

--Iteration 12--
unicode(3) "bar"

--Iteration 13--
unicode(3) "bar"

--Iteration 14--
unicode(3) "bar"

--Iteration 15--
unicode(3) "bar"

--Iteration 16--
unicode(3) "bar"

--Iteration 17--
unicode(3) "bar"

--Iteration 18--
unicode(3) "bar"

--Iteration 19--
unicode(3) "bar"

--Iteration 20--
unicode(3) ".gz"

--Iteration 21--
unicode(3) ".gz"

--Iteration 22--
unicode(3) ".gz"

--Iteration 23--
unicode(3) ".gz"

--Iteration 24--
unicode(3) ".gz"

--Iteration 25--
unicode(3) ".gz"

--Iteration 26--
string(3) "bar"

--Iteration 27--
string(3) "bar"

--Iteration 28--
string(3) "bar"

--Iteration 29--
string(3) "bar"

--Iteration 30--
string(3) "bar"

--Iteration 31--
string(3) "bar"

--Iteration 32--
string(6) "bar.gz"
Done
