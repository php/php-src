--TEST--
Test pathinfo() function: basic functionality
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Not valid for Windows");
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
--EXPECTF--
*** Testing basic functions of pathinfo() ***
-- Iteration 1 --
%unicode|string%(1) "."
%unicode|string%(10) "c:\..\dir1"
%unicode|string%(5) "\dir1"
%unicode|string%(4) "c:\."
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(10) "c:\..\dir1"
  [%u|b%"extension"]=>
  %unicode|string%(5) "\dir1"
  [%u|b%"filename"]=>
  %unicode|string%(4) "c:\."
}
-- Iteration 2 --
%unicode|string%(1) "."
%unicode|string%(33) "c:\test\..\test2\.\adir\afile.txt"
%unicode|string%(3) "txt"
%unicode|string%(29) "c:\test\..\test2\.\adir\afile"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(33) "c:\test\..\test2\.\adir\afile.txt"
  [%u|b%"extension"]=>
  %unicode|string%(3) "txt"
  [%u|b%"filename"]=>
  %unicode|string%(29) "c:\test\..\test2\.\adir\afile"
}
-- Iteration 3 --
%unicode|string%(22) "/usr/include/../arpa/."
%unicode|string%(6) "inet.h"
%unicode|string%(1) "h"
%unicode|string%(4) "inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(22) "/usr/include/../arpa/."
  [%u|b%"basename"]=>
  %unicode|string%(6) "inet.h"
  [%u|b%"extension"]=>
  %unicode|string%(1) "h"
  [%u|b%"filename"]=>
  %unicode|string%(4) "inet"
}
-- Iteration 4 --
%unicode|string%(1) "."
%unicode|string%(23) "c:\test\adir\afile..txt"
%unicode|string%(3) "txt"
%unicode|string%(19) "c:\test\adir\afile."
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(23) "c:\test\adir\afile..txt"
  [%u|b%"extension"]=>
  %unicode|string%(3) "txt"
  [%u|b%"filename"]=>
  %unicode|string%(19) "c:\test\adir\afile."
}
-- Iteration 5 --
%unicode|string%(17) "/usr/include/arpa"
%unicode|string%(7) "inet..h"
%unicode|string%(1) "h"
%unicode|string%(5) "inet."
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(17) "/usr/include/arpa"
  [%u|b%"basename"]=>
  %unicode|string%(7) "inet..h"
  [%u|b%"extension"]=>
  %unicode|string%(1) "h"
  [%u|b%"filename"]=>
  %unicode|string%(5) "inet."
}
-- Iteration 6 --
%unicode|string%(1) "."
%unicode|string%(19) "c:\test\adir\afile."
%unicode|string%(0) ""
%unicode|string%(18) "c:\test\adir\afile"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(19) "c:\test\adir\afile."
  [%u|b%"extension"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(18) "c:\test\adir\afile"
}
-- Iteration 7 --
%unicode|string%(17) "/usr/include/arpa"
%unicode|string%(5) "inet."
%unicode|string%(0) ""
%unicode|string%(4) "inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(17) "/usr/include/arpa"
  [%u|b%"basename"]=>
  %unicode|string%(5) "inet."
  [%u|b%"extension"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(4) "inet"
}
-- Iteration 8 --
%unicode|string%(17) "/usr/include/arpa"
%unicode|string%(6) "inet,h"
%unicode|string%(0) ""
%unicode|string%(6) "inet,h"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(17) "/usr/include/arpa"
  [%u|b%"basename"]=>
  %unicode|string%(6) "inet,h"
  [%u|b%"filename"]=>
  %unicode|string%(6) "inet,h"
}
-- Iteration 9 --
%unicode|string%(1) "."
%unicode|string%(11) "c:afile.txt"
%unicode|string%(3) "txt"
%unicode|string%(7) "c:afile"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(11) "c:afile.txt"
  [%u|b%"extension"]=>
  %unicode|string%(3) "txt"
  [%u|b%"filename"]=>
  %unicode|string%(7) "c:afile"
}
-- Iteration 10 --
%unicode|string%(1) "."
%unicode|string%(22) "..\.\..\test\afile.txt"
%unicode|string%(3) "txt"
%unicode|string%(18) "..\.\..\test\afile"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(22) "..\.\..\test\afile.txt"
  [%u|b%"extension"]=>
  %unicode|string%(3) "txt"
  [%u|b%"filename"]=>
  %unicode|string%(18) "..\.\..\test\afile"
}
-- Iteration 11 --
%unicode|string%(12) ".././../test"
%unicode|string%(5) "afile"
%unicode|string%(0) ""
%unicode|string%(5) "afile"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(12) ".././../test"
  [%u|b%"basename"]=>
  %unicode|string%(5) "afile"
  [%u|b%"filename"]=>
  %unicode|string%(5) "afile"
}
-- Iteration 12 --
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
-- Iteration 13 --
%unicode|string%(1) "."
%unicode|string%(2) ".."
%unicode|string%(0) ""
%unicode|string%(1) "."
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(2) ".."
  [%u|b%"extension"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(1) "."
}
-- Iteration 14 --
%unicode|string%(1) "."
%unicode|string%(3) "..."
%unicode|string%(0) ""
%unicode|string%(2) ".."
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(3) "..."
  [%u|b%"extension"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(2) ".."
}
-- Iteration 15 --
%unicode|string%(12) "/usr/lib/..."
%unicode|string%(5) "afile"
%unicode|string%(0) ""
%unicode|string%(5) "afile"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(12) "/usr/lib/..."
  [%u|b%"basename"]=>
  %unicode|string%(5) "afile"
  [%u|b%"filename"]=>
  %unicode|string%(5) "afile"
}
Done

