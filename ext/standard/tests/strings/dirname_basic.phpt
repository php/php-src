--TEST--
Test dirname() function : basic funtionality
--FILE--
<?php
/* Prototype: string dirname ( string $path );
   Description: Returns directory name component of path.
*/
$file_paths = array (
  /* simple paths */
  "bar",
  "/foo/bar",
  "foo/bar",
  "/bar",
  "bar/",
  "/bar/",
  "/foo/bar/",
  "foo/bar/",
  "/bar/",
  
  /* path with only files and trailing slashes*/
  "/foo/bar.gz",
  "foo/bar.gz",
  "bar.gz",
  "bar.gz/",
  "/bar.gz",  
  "/bar.gz/",
  "/foo/bar.gz/",
  "foo/bar.gz/",
  "/bar.gz/",  
 
  /* path with file extension and trailing slashes */
  "/.gz",
  ".gz",
  "/foo/.gz",
  ".gz/",
  "/foo/.gz/",
  "foo/.gz/",

  /* paths with binary value to check if the function is binary safe*/
  b"foo/bar",
  b"/foo/bar/",
  b"/foo/bar",
  b"foo/bar/",
  b"/foo/bar/t.gz"
);

function check_dirname( $paths ) {
   $loop_counter = 0;
   $noOfPaths = count($paths);
   for( ; $loop_counter < $noOfPaths; $loop_counter++ ) {
     echo "\n--Iteration ";
     echo $loop_counter + 1; 
     echo " --\n";
     var_dump( dirname($paths[$loop_counter]) );
   }
}

echo "*** Testing basic operations ***\n";
check_dirname( $file_paths );

echo "Done\n";
?>
--EXPECTREGEX--
\*\*\* Testing basic operations \*\*\*

--Iteration 1 --
unicode\(1\) "."

--Iteration 2 --
unicode\(4\) "(\\|\/)foo"

--Iteration 3 --
unicode\(3\) "foo"

--Iteration 4 --
unicode\(1\) "(\\|\/)"

--Iteration 5 --
unicode\(1\) "."

--Iteration 6 --
unicode\(1\) "(\\|\/)"

--Iteration 7 --
unicode\(4\) "(\\|\/)foo"

--Iteration 8 --
unicode\(3\) "foo"

--Iteration 9 --
unicode\(1\) "(\\|\/)"

--Iteration 10 --
unicode\(4\) "(\\|\/)foo"

--Iteration 11 --
unicode\(3\) "foo"

--Iteration 12 --
unicode\(1\) "."

--Iteration 13 --
unicode\(1\) "."

--Iteration 14 --
unicode\(1\) "(\\|\/)"

--Iteration 15 --
unicode\(1\) "(\\|\/)"

--Iteration 16 --
unicode\(4\) "(\\|\/)foo"

--Iteration 17 --
unicode\(3\) "foo"

--Iteration 18 --
unicode\(1\) "(\\|\/)"

--Iteration 19 --
unicode\(1\) "(\\|\/)"

--Iteration 20 --
unicode\(1\) "."

--Iteration 21 --
unicode\(4\) "(\\|\/)foo"

--Iteration 22 --
unicode\(1\) "."

--Iteration 23 --
unicode\(4\) "(\\|\/)foo"

--Iteration 24 --
unicode\(3\) "foo"

--Iteration 25 --
string\(3\) "foo"

--Iteration 26 --
string\(4\) "(\\|\/)foo"

--Iteration 27 --
string\(4\) "(\\|\/)foo"

--Iteration 28 --
string\(3\) "foo"

--Iteration 29 --
string\(8\) "(\\|\/)foo(\\|\/)bar"
Done
