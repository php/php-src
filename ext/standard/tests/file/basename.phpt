--TEST--
basename
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/*
 * Function is implemented in ext/standard/string.c
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
  array("foo".chr(0)."bar"),
  array("/foo".chr(0)."bar"),
  array("/foo".chr(0)."bar/"),
  array("foo".chr(0)."bar/"),
  array("foo".chr(0)."bar/test"),
  array("/foo".chr(0)."bar/bar.gz", ".gz"),
  array("/foo".chr(0)."bar/bar.gz")
);

$file_path_variations = array (
  /* paths with shortcut home dir char, with suffix variation */
  array("~/home/user/bar"),
  array("~/home/user/bar", ""),
  array("~/home/user/bar", ' '),
  array("~/home/user/bar.tar", ".tar"),
  array("~/home/user/bar.tar", "~"),
  array("~/home/user/bar.tar/", "~"),
  array("~/home/user/bar.tar/", ""),
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

  /* path with spaces */
  array(" "),
  array(' '),

  /* empty paths */
  array(""),
  array(''),
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
string(7) "bar.tar"

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
string(1) "t"

--Iteration 13--
string(7) "tbar.gz"

--Iteration 14--
string(7) "tbar.gz"

--Iteration 15--
string(7) "tbar.gz"

--Iteration 16--
string(7) "tbar.gz"

--Iteration 17--
string(10) "My Pics.gz"

--Iteration 18--
string(10) "My Pics.gz"

--Iteration 19--
string(3) "My "

--Iteration 20--
string(7) "My Pics"

--Iteration 21--
string(10) "My Pics.gz"

--Iteration 22--
string(10) "My Pics.gz"

--Iteration 23--
string(10) "My Pics.gz"

--Iteration 24--
string(10) "My Pics.gz"

--Iteration 25--
string(4) "10.5"

--Iteration 26--
string(2) "10"

--Iteration 27--
string(4) "10.5"

--Iteration 28--
string(2) "10"

--Iteration 29--
string(2) "10"

--Iteration 30--
string(4) "10.5"

--Iteration 31--
string(4) "10.5"

--Iteration 32--
string(5) "10.gz"

--Iteration 33--
string(1) "0"

--Iteration 34--
string(1) "0"

--Iteration 35--
string(6) "bar.gz"

--Iteration 36--
string(6) "bar.gz"

--Iteration 37--
string(6) "bar.gz"

--Iteration 38--
string(1) " "

--Iteration 39--
string(1) " "

--Iteration 40--
string(1) " "

--Iteration 41--
string(1) " "

--Iteration 42--
string(0) ""

--Iteration 43--
string(0) ""
Done
