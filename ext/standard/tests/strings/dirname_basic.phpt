--TEST--
Test dirname() function : basic functionality
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
  "foo".chr(0)."bar",
  "/foo".chr(0)."bar/",
  "/foo".chr(0)."bar",
  "foo".chr(0)."bar/",
  "/foo".chr(0)."bar/t.gz"
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
string\(1\) "."

--Iteration 2 --
string\(4\) "(\\|\/)foo"

--Iteration 3 --
string\(3\) "foo"

--Iteration 4 --
string\(1\) "(\\|\/)"

--Iteration 5 --
string\(1\) "."

--Iteration 6 --
string\(1\) "(\\|\/)"

--Iteration 7 --
string\(4\) "(\\|\/)foo"

--Iteration 8 --
string\(3\) "foo"

--Iteration 9 --
string\(1\) "(\\|\/)"

--Iteration 10 --
string\(4\) "(\\|\/)foo"

--Iteration 11 --
string\(3\) "foo"

--Iteration 12 --
string\(1\) "."

--Iteration 13 --
string\(1\) "."

--Iteration 14 --
string\(1\) "(\\|\/)"

--Iteration 15 --
string\(1\) "(\\|\/)"

--Iteration 16 --
string\(4\) "(\\|\/)foo"

--Iteration 17 --
string\(3\) "foo"

--Iteration 18 --
string\(1\) "(\\|\/)"

--Iteration 19 --
string\(1\) "(\\|\/)"

--Iteration 20 --
string\(1\) "."

--Iteration 21 --
string\(4\) "(\\|\/)foo"

--Iteration 22 --
string\(1\) "."

--Iteration 23 --
string\(4\) "(\\|\/)foo"

--Iteration 24 --
string\(3\) "foo"

--Iteration 25 --
string\(1\) "."

--Iteration 26 --
string\(1\) "(\\|\/)"

--Iteration 27 --
string\(1\) "(\\|\/)"

--Iteration 28 --
string\(1\) "."

--Iteration 29 --
string\(8\) "(\\|\/)foo\x00bar"
Done
