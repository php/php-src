--TEST--
Test basename() function : usage variations
--FILE--
<?php
/* Prototype: string basename ( string $path [, string $suffix] );
   Description: Given a string containing a path to a file,
                this function will return the base name of the file. 
                If the filename ends in suffix this will also be cut off.
*/
$file_path_variations = array (
  /* paths with shortcut home dir char, with suffix variation */
  array("~/home/user/bar"),
  array("~/home/user/bar", ""),
  array("~/home/user/bar", NULL),
  array("~/home/user/bar", ' '),
  array("~/home/user/bar.tar", ".tar"),
  array("~/home/user/bar.tar", "~"),
  array("~/home/user/bar.tar/", "~"),
  array("~/home/user/bar.tar/", ""),
  array("~/home/user/bar.tar", NULL),
  array("~/home/user/bar.tar", ''),
  array("~/home/user/bar.tar", " "),

  /* paths with hostname:dir notation, with suffix variation */
  array("hostname:/home/usr/bar.tar"),
  array("hostname:/home/user/bar.tar", "home"),
  array("hostname:/home/user/tbar.gz", "bar.gz"),
  array("hostname:/home/user/tbar.gz", "/bar.gz"),
  array("hostname:/home/user/tbar.gz", "/bar.gz/"),
  array("hostname:/home/user/tbar.gz/", "/bar.gz/"),
  array("hostname:/home/user/tbar.gz/", "/bar.gz/"),
  array("hostname:/home/user/My Pics.gz/", "/bar.gz/"),
  array("hostname:/home/user/My Pics.gz/", "Pics.gz/"),
  array("hostname:/home/user/My Pics.gz/", "Pics.gz"),
  array("hostname:/home/user/My Pics.gz/", ".gz"),
  array("hostname:/home/user/My Pics.gz/"),
  array("hostname:/home/user/My Pics.gz/", NULL),
  array("hostname:/home/user/My Pics.gz/", ' '),
  array("hostname:/home/user/My Pics.gz/", ''),
  array("hostname:/home/user/My Pics.gz/", "My Pics.gz"),

  /* paths with numeirc strings */
  array("10.5"),
  array("10.5", ".5"),
  array("10.5", "10.5"),
  array("10"),
  array("105", "5"),
  array("/10.5"),
  array("10.5/"),
  array("10/10.gz"),
  array("0"),
  array('0'),

  /* paths and suffix given as same */
  array("bar.gz", "bar.gz"),
  array("/bar.gz", "/bar.gz"),
  array("/bar.gz/", "/bar.gz/"),
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

echo "*** Testing possible variations in path and suffix ***\n";
check_basename( $file_path_variations );

echo "Done\n";

--EXPECTF--
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
string(7) "bar.tar"

--Iteration 13--
string(7) "bar.tar"

--Iteration 14--
string(1) "t"

--Iteration 15--
string(7) "tbar.gz"

--Iteration 16--
string(7) "tbar.gz"

--Iteration 17--
string(7) "tbar.gz"

--Iteration 18--
string(7) "tbar.gz"

--Iteration 19--
string(10) "My Pics.gz"

--Iteration 20--
string(10) "My Pics.gz"

--Iteration 21--
string(3) "My "

--Iteration 22--
string(7) "My Pics"

--Iteration 23--
string(10) "My Pics.gz"

--Iteration 24--
string(10) "My Pics.gz"

--Iteration 25--
string(10) "My Pics.gz"

--Iteration 26--
string(10) "My Pics.gz"

--Iteration 27--
string(10) "My Pics.gz"

--Iteration 28--
string(4) "10.5"

--Iteration 29--
string(2) "10"

--Iteration 30--
string(4) "10.5"

--Iteration 31--
string(2) "10"

--Iteration 32--
string(2) "10"

--Iteration 33--
string(4) "10.5"

--Iteration 34--
string(4) "10.5"

--Iteration 35--
string(5) "10.gz"

--Iteration 36--
string(1) "0"

--Iteration 37--
string(1) "0"

--Iteration 38--
string(6) "bar.gz"

--Iteration 39--
string(6) "bar.gz"

--Iteration 40--
string(6) "bar.gz"

--Iteration 41--
string(1) " "

--Iteration 42--
string(1) " "

--Iteration 43--
string(0) ""

--Iteration 44--
string(1) " "

--Iteration 45--
string(1) " "

--Iteration 46--
string(0) ""

--Iteration 47--
string(0) ""

--Iteration 48--
string(0) ""
Done
