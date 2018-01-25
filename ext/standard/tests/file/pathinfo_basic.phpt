--TEST--
Test pathinfo() function: basic functionality
--FILE--
<?php
/* Prototype: mixed pathinfo ( string $path [, int $options] );
   Description: Returns information about a file path
*/

echo "*** Testing basic functions of pathinfo() ***\n";

$file_path = dirname(__FILE__);

$paths = array (
  /* Testing basic file notation */
  "$file_path/foo/symlink.link",
  "www.example.co.in",
  "/var/www/html/example.html",
  "/dir/test.tar.gz",

  /* Testing a file with trailing slash */
  "$file_path/foo/symlink.link/",

  /* Testing file with double slashes */
  "$file_path/foo//symlink.link",
  "$file_path/foo//symlink.link",
  "$file_path/foo//symlink.link//",

  /* Testing file with trailing double slashes */
  "$file_path/foo/symlink.link//",

  /* Testing Binary safe files */
  "$file_path/foo".chr(47)."symlink.link",
  "$file_path".chr(47)."foo/symlink.link",
  "$file_path".chr(47)."foo".chr(47)."symlink.link",
  "$file_path/foo/symlink.link",

  /* Testing directories */
  ".",  // current directory
  "$file_path/foo/",
  "$file_path/foo//",
  "$file_path/../foo/",
  "../foo/bar",
  "./foo/bar",
  "//foo//bar//",

  /* Testing with homedir notation */
  "~/PHP/php5.2.0/",

  /* Testing normal directory notation */
  "/home/example/test/",
  "http://httpd.apache.org/core.html#acceptpathinfo"
);

$counter = 1;
/* loop through $paths to test each $path in the above array */
foreach($paths as $path) {
  echo "-- Iteration $counter --\n";
  var_dump( pathinfo($path, PATHINFO_DIRNAME) );
  var_dump( pathinfo($path, PATHINFO_BASENAME) );
  var_dump( pathinfo($path, PATHINFO_EXTENSION) );
  var_dump( pathinfo($path, PATHINFO_FILENAME) );
  var_dump( pathinfo($path) );
  $counter++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing basic functions of pathinfo() ***
-- Iteration 1 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 2 --
string(1) "."
string(17) "www.example.co.in"
string(2) "in"
string(14) "www.example.co"
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(17) "www.example.co.in"
  ["extension"]=>
  string(2) "in"
  ["filename"]=>
  string(14) "www.example.co"
}
-- Iteration 3 --
string(13) "/var/www/html"
string(12) "example.html"
string(4) "html"
string(7) "example"
array(4) {
  ["dirname"]=>
  string(13) "/var/www/html"
  ["basename"]=>
  string(12) "example.html"
  ["extension"]=>
  string(4) "html"
  ["filename"]=>
  string(7) "example"
}
-- Iteration 4 --
string(4) "/dir"
string(11) "test.tar.gz"
string(2) "gz"
string(8) "test.tar"
array(4) {
  ["dirname"]=>
  string(4) "/dir"
  ["basename"]=>
  string(11) "test.tar.gz"
  ["extension"]=>
  string(2) "gz"
  ["filename"]=>
  string(8) "test.tar"
}
-- Iteration 5 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 6 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 7 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 8 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 9 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 10 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 11 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 12 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 13 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  ["dirname"]=>
  string(%d) "%s/foo"
  ["basename"]=>
  string(12) "symlink.link"
  ["extension"]=>
  string(4) "link"
  ["filename"]=>
  string(7) "symlink"
}
-- Iteration 14 --
string(1) "."
string(1) "."
string(0) ""
string(0) ""
array(4) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(1) "."
  ["extension"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
-- Iteration 15 --
string(%d) "%s"
string(3) "foo"
string(0) ""
string(3) "foo"
array(3) {
  ["dirname"]=>
  string(%d) "%s"
  ["basename"]=>
  string(3) "foo"
  ["filename"]=>
  string(3) "foo"
}
-- Iteration 16 --
string(%d) "%s"
string(3) "foo"
string(0) ""
string(3) "foo"
array(3) {
  ["dirname"]=>
  string(%d) "%s"
  ["basename"]=>
  string(3) "foo"
  ["filename"]=>
  string(3) "foo"
}
-- Iteration 17 --
string(%d) "%s/.."
string(3) "foo"
string(0) ""
string(3) "foo"
array(3) {
  ["dirname"]=>
  string(%d) "%s/.."
  ["basename"]=>
  string(3) "foo"
  ["filename"]=>
  string(3) "foo"
}
-- Iteration 18 --
string(6) "../foo"
string(3) "bar"
string(0) ""
string(3) "bar"
array(3) {
  ["dirname"]=>
  string(6) "../foo"
  ["basename"]=>
  string(3) "bar"
  ["filename"]=>
  string(3) "bar"
}
-- Iteration 19 --
string(5) "./foo"
string(3) "bar"
string(0) ""
string(3) "bar"
array(3) {
  ["dirname"]=>
  string(5) "./foo"
  ["basename"]=>
  string(3) "bar"
  ["filename"]=>
  string(3) "bar"
}
-- Iteration 20 --
string(5) "//foo"
string(3) "bar"
string(0) ""
string(3) "bar"
array(3) {
  ["dirname"]=>
  string(5) "//foo"
  ["basename"]=>
  string(3) "bar"
  ["filename"]=>
  string(3) "bar"
}
-- Iteration 21 --
string(5) "~/PHP"
string(8) "php5.2.0"
string(1) "0"
string(6) "php5.2"
array(4) {
  ["dirname"]=>
  string(5) "~/PHP"
  ["basename"]=>
  string(8) "php5.2.0"
  ["extension"]=>
  string(1) "0"
  ["filename"]=>
  string(6) "php5.2"
}
-- Iteration 22 --
string(13) "/home/example"
string(4) "test"
string(0) ""
string(4) "test"
array(3) {
  ["dirname"]=>
  string(13) "/home/example"
  ["basename"]=>
  string(4) "test"
  ["filename"]=>
  string(4) "test"
}
-- Iteration 23 --
string(23) "http://httpd.apache.org"
string(24) "core.html#acceptpathinfo"
string(19) "html#acceptpathinfo"
string(4) "core"
array(4) {
  ["dirname"]=>
  string(23) "http://httpd.apache.org"
  ["basename"]=>
  string(24) "core.html#acceptpathinfo"
  ["extension"]=>
  string(19) "html#acceptpathinfo"
  ["filename"]=>
  string(4) "core"
}
Done
