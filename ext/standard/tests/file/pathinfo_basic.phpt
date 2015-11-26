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
  b"$file_path/foo/symlink.link",

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
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 2 --
%unicode|string%(1) "."
%unicode|string%(17) "www.example.co.in"
%unicode|string%(2) "in"
%unicode|string%(14) "www.example.co"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(17) "www.example.co.in"
  [%u|b%"extension"]=>
  %unicode|string%(2) "in"
  [%u|b%"filename"]=>
  %unicode|string%(14) "www.example.co"
}
-- Iteration 3 --
%unicode|string%(13) "/var/www/html"
%unicode|string%(12) "example.html"
%unicode|string%(4) "html"
%unicode|string%(7) "example"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(13) "/var/www/html"
  [%u|b%"basename"]=>
  %unicode|string%(12) "example.html"
  [%u|b%"extension"]=>
  %unicode|string%(4) "html"
  [%u|b%"filename"]=>
  %unicode|string%(7) "example"
}
-- Iteration 4 --
%unicode|string%(4) "/dir"
%unicode|string%(11) "test.tar.gz"
%unicode|string%(2) "gz"
%unicode|string%(8) "test.tar"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(4) "/dir"
  [%u|b%"basename"]=>
  %unicode|string%(11) "test.tar.gz"
  [%u|b%"extension"]=>
  %unicode|string%(2) "gz"
  [%u|b%"filename"]=>
  %unicode|string%(8) "test.tar"
}
-- Iteration 5 --
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 6 --
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 7 --
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 8 --
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 9 --
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 10 --
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 11 --
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 12 --
%unicode|string%(%d) "%s/foo"
%unicode|string%(12) "symlink.link"
%unicode|string%(4) "link"
%unicode|string%(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/foo"
  [%u|b%"basename"]=>
  %unicode|string%(12) "symlink.link"
  [%u|b%"extension"]=>
  %unicode|string%(4) "link"
  [%u|b%"filename"]=>
  %unicode|string%(7) "symlink"
}
-- Iteration 13 --
string(%d) "%s/foo"
string(12) "symlink.link"
string(4) "link"
string(7) "symlink"
array(4) {
  [%u|b%"dirname"]=>
  string(%d) "%s/foo"
  [%u|b%"basename"]=>
  string(12) "symlink.link"
  [%u|b%"extension"]=>
  string(4) "link"
  [%u|b%"filename"]=>
  string(7) "symlink"
}
-- Iteration 14 --
%unicode|string%(1) "."
%unicode|string%(1) "."
%unicode|string%(0) ""
%unicode|string%(0) ""
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(1) "."
  [%u|b%"extension"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
-- Iteration 15 --
%unicode|string%(%d) "%s"
%unicode|string%(3) "foo"
%unicode|string%(0) ""
%unicode|string%(3) "foo"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"basename"]=>
  %unicode|string%(3) "foo"
  [%u|b%"filename"]=>
  %unicode|string%(3) "foo"
}
-- Iteration 16 --
%unicode|string%(%d) "%s"
%unicode|string%(3) "foo"
%unicode|string%(0) ""
%unicode|string%(3) "foo"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"basename"]=>
  %unicode|string%(3) "foo"
  [%u|b%"filename"]=>
  %unicode|string%(3) "foo"
}
-- Iteration 17 --
%unicode|string%(%d) "%s/.."
%unicode|string%(3) "foo"
%unicode|string%(0) ""
%unicode|string%(3) "foo"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(%d) "%s/.."
  [%u|b%"basename"]=>
  %unicode|string%(3) "foo"
  [%u|b%"filename"]=>
  %unicode|string%(3) "foo"
}
-- Iteration 18 --
%unicode|string%(6) "../foo"
%unicode|string%(3) "bar"
%unicode|string%(0) ""
%unicode|string%(3) "bar"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(6) "../foo"
  [%u|b%"basename"]=>
  %unicode|string%(3) "bar"
  [%u|b%"filename"]=>
  %unicode|string%(3) "bar"
}
-- Iteration 19 --
%unicode|string%(5) "./foo"
%unicode|string%(3) "bar"
%unicode|string%(0) ""
%unicode|string%(3) "bar"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(5) "./foo"
  [%u|b%"basename"]=>
  %unicode|string%(3) "bar"
  [%u|b%"filename"]=>
  %unicode|string%(3) "bar"
}
-- Iteration 20 --
%unicode|string%(5) "//foo"
%unicode|string%(3) "bar"
%unicode|string%(0) ""
%unicode|string%(3) "bar"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(5) "//foo"
  [%u|b%"basename"]=>
  %unicode|string%(3) "bar"
  [%u|b%"filename"]=>
  %unicode|string%(3) "bar"
}
-- Iteration 21 --
%unicode|string%(5) "~/PHP"
%unicode|string%(8) "php5.2.0"
%unicode|string%(1) "0"
%unicode|string%(6) "php5.2"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(5) "~/PHP"
  [%u|b%"basename"]=>
  %unicode|string%(8) "php5.2.0"
  [%u|b%"extension"]=>
  %unicode|string%(1) "0"
  [%u|b%"filename"]=>
  %unicode|string%(6) "php5.2"
}
-- Iteration 22 --
%unicode|string%(13) "/home/example"
%unicode|string%(4) "test"
%unicode|string%(0) ""
%unicode|string%(4) "test"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(13) "/home/example"
  [%u|b%"basename"]=>
  %unicode|string%(4) "test"
  [%u|b%"filename"]=>
  %unicode|string%(4) "test"
}
-- Iteration 23 --
%unicode|string%(23) "http://httpd.apache.org"
%unicode|string%(24) "core.html#acceptpathinfo"
%unicode|string%(19) "html#acceptpathinfo"
%unicode|string%(4) "core"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(23) "http://httpd.apache.org"
  [%u|b%"basename"]=>
  %unicode|string%(24) "core.html#acceptpathinfo"
  [%u|b%"extension"]=>
  %unicode|string%(19) "html#acceptpathinfo"
  [%u|b%"filename"]=>
  %unicode|string%(4) "core"
}
Done
