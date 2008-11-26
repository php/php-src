--TEST--
Test pathinfo() function: basic functionality
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only valid for Windows");
?>
--FILE--
<?php
/* Prototype: mixed pathinfo ( string $path [, int $options] );
   Description: Returns information about a file path
*/

echo "*** Testing basic functions of pathinfo() ***\n";

$paths = array (
			'c:\..\dir1',
			'c:\test\..\test2\.\adir\afile.txt',
			'/usr/include/../arpa/./inet.h',
			'c:\test\adir\afile..txt',
			'/usr/include/arpa/inet..h',
			'c:\test\adir\afile.',
			'/usr/include/arpa/inet.',
			'/usr/include/arpa/inet,h',
			'c:afile.txt',
			'..\.\..\test\afile.txt',
			'.././../test/afile',
			'.',
			'..',
			'...',
			'/usr/lib/.../afile'
						
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
--EXPECT--
*** Testing basic functions of pathinfo() ***
-- Iteration 1 --
unicode(5) "c:\.."
unicode(4) "dir1"
string(0) ""
unicode(4) "dir1"
array(3) {
  [u"dirname"]=>
  unicode(5) "c:\.."
  [u"basename"]=>
  unicode(4) "dir1"
  [u"filename"]=>
  unicode(4) "dir1"
}
-- Iteration 2 --
unicode(23) "c:\test\..\test2\.\adir"
unicode(9) "afile.txt"
unicode(3) "txt"
unicode(5) "afile"
array(4) {
  [u"dirname"]=>
  unicode(23) "c:\test\..\test2\.\adir"
  [u"basename"]=>
  unicode(9) "afile.txt"
  [u"extension"]=>
  unicode(3) "txt"
  [u"filename"]=>
  unicode(5) "afile"
}
-- Iteration 3 --
unicode(22) "/usr/include/../arpa/."
unicode(6) "inet.h"
unicode(1) "h"
unicode(4) "inet"
array(4) {
  [u"dirname"]=>
  unicode(22) "/usr/include/../arpa/."
  [u"basename"]=>
  unicode(6) "inet.h"
  [u"extension"]=>
  unicode(1) "h"
  [u"filename"]=>
  unicode(4) "inet"
}
-- Iteration 4 --
unicode(12) "c:\test\adir"
unicode(10) "afile..txt"
unicode(3) "txt"
unicode(6) "afile."
array(4) {
  [u"dirname"]=>
  unicode(12) "c:\test\adir"
  [u"basename"]=>
  unicode(10) "afile..txt"
  [u"extension"]=>
  unicode(3) "txt"
  [u"filename"]=>
  unicode(6) "afile."
}
-- Iteration 5 --
unicode(17) "/usr/include/arpa"
unicode(7) "inet..h"
unicode(1) "h"
unicode(5) "inet."
array(4) {
  [u"dirname"]=>
  unicode(17) "/usr/include/arpa"
  [u"basename"]=>
  unicode(7) "inet..h"
  [u"extension"]=>
  unicode(1) "h"
  [u"filename"]=>
  unicode(5) "inet."
}
-- Iteration 6 --
unicode(12) "c:\test\adir"
unicode(6) "afile."
unicode(0) ""
unicode(5) "afile"
array(4) {
  [u"dirname"]=>
  unicode(12) "c:\test\adir"
  [u"basename"]=>
  unicode(6) "afile."
  [u"extension"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(5) "afile"
}
-- Iteration 7 --
unicode(17) "/usr/include/arpa"
unicode(5) "inet."
unicode(0) ""
unicode(4) "inet"
array(4) {
  [u"dirname"]=>
  unicode(17) "/usr/include/arpa"
  [u"basename"]=>
  unicode(5) "inet."
  [u"extension"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(4) "inet"
}
-- Iteration 8 --
unicode(17) "/usr/include/arpa"
unicode(6) "inet,h"
string(0) ""
unicode(6) "inet,h"
array(3) {
  [u"dirname"]=>
  unicode(17) "/usr/include/arpa"
  [u"basename"]=>
  unicode(6) "inet,h"
  [u"filename"]=>
  unicode(6) "inet,h"
}
-- Iteration 9 --
unicode(3) "c:."
unicode(11) "c:afile.txt"
unicode(3) "txt"
unicode(7) "c:afile"
array(4) {
  [u"dirname"]=>
  unicode(3) "c:."
  [u"basename"]=>
  unicode(11) "c:afile.txt"
  [u"extension"]=>
  unicode(3) "txt"
  [u"filename"]=>
  unicode(7) "c:afile"
}
-- Iteration 10 --
unicode(12) "..\.\..\test"
unicode(9) "afile.txt"
unicode(3) "txt"
unicode(5) "afile"
array(4) {
  [u"dirname"]=>
  unicode(12) "..\.\..\test"
  [u"basename"]=>
  unicode(9) "afile.txt"
  [u"extension"]=>
  unicode(3) "txt"
  [u"filename"]=>
  unicode(5) "afile"
}
-- Iteration 11 --
unicode(12) ".././../test"
unicode(5) "afile"
string(0) ""
unicode(5) "afile"
array(3) {
  [u"dirname"]=>
  unicode(12) ".././../test"
  [u"basename"]=>
  unicode(5) "afile"
  [u"filename"]=>
  unicode(5) "afile"
}
-- Iteration 12 --
unicode(1) "."
unicode(1) "."
unicode(0) ""
unicode(0) ""
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(1) "."
  [u"extension"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
-- Iteration 13 --
unicode(1) "."
unicode(2) ".."
unicode(0) ""
unicode(1) "."
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(2) ".."
  [u"extension"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(1) "."
}
-- Iteration 14 --
unicode(1) "."
unicode(3) "..."
unicode(0) ""
unicode(2) ".."
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(3) "..."
  [u"extension"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(2) ".."
}
-- Iteration 15 --
unicode(12) "/usr/lib/..."
unicode(5) "afile"
string(0) ""
unicode(5) "afile"
array(3) {
  [u"dirname"]=>
  unicode(12) "/usr/lib/..."
  [u"basename"]=>
  unicode(5) "afile"
  [u"filename"]=>
  unicode(5) "afile"
}
Done

