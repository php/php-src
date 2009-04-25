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
				'',
 			' ',
			'c:',
			'c:\\',
			'c:/',
			'afile',
			'c:\test\adir',
			'c:\test\adir\\',
			'/usr/include/arpa',
			'/usr/include/arpa/',
			'usr/include/arpa',
			'usr/include/arpa/',			
			'c:\test\afile',
			'c:\\test\\afile',
			'c://test//afile',
			'c:\test\afile\\',
			'c:\test\prog.exe',
			'c:\\test\\prog.exe',
			'c:/test/prog.exe',			
			'/usr/include/arpa/inet.h',
			'//usr/include//arpa/inet.h',
			'\\',
			'\\\\',
			'/',
			'//',
			'///',
			'/usr/include/arpa/inet.h',
			'c:\windows/system32\drivers/etc\hosts',
			'/usr\include/arpa\inet.h',
			'   c:\test\adir\afile.txt',
			'c:\test\adir\afile.txt   ',
			'   c:\test\adir\afile.txt   ',
			'   /usr/include/arpa/inet.h',
			'/usr/include/arpa/inet.h   ',
			'   /usr/include/arpa/inet.h   ',
			' c:',
			'		c:\test\adir\afile.txt',
			'/usr',
			'/usr/'
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
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
array(2) {
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
-- Iteration 2 --
%unicode|string%(1) "."
%unicode|string%(1) " "
%unicode|string%(0) ""
%unicode|string%(1) " "
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(1) " "
  [%u|b%"filename"]=>
  %unicode|string%(1) " "
}
-- Iteration 3 --
%unicode|string%(1) "."
%unicode|string%(2) "c:"
%unicode|string%(0) ""
%unicode|string%(2) "c:"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(2) "c:"
  [%u|b%"filename"]=>
  %unicode|string%(2) "c:"
}
-- Iteration 4 --
%unicode|string%(1) "."
%unicode|string%(3) "c:\"
%unicode|string%(0) ""
%unicode|string%(3) "c:\"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(3) "c:\"
  [%u|b%"filename"]=>
  %unicode|string%(3) "c:\"
}
-- Iteration 5 --
%unicode|string%(1) "."
%unicode|string%(2) "c:"
%unicode|string%(0) ""
%unicode|string%(2) "c:"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(2) "c:"
  [%u|b%"filename"]=>
  %unicode|string%(2) "c:"
}
-- Iteration 6 --
%unicode|string%(1) "."
%unicode|string%(5) "afile"
%unicode|string%(0) ""
%unicode|string%(5) "afile"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(5) "afile"
  [%u|b%"filename"]=>
  %unicode|string%(5) "afile"
}
-- Iteration 7 --
%unicode|string%(1) "."
%unicode|string%(12) "c:\test\adir"
%unicode|string%(0) ""
%unicode|string%(12) "c:\test\adir"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(12) "c:\test\adir"
  [%u|b%"filename"]=>
  %unicode|string%(12) "c:\test\adir"
}
-- Iteration 8 --
%unicode|string%(1) "."
%unicode|string%(13) "c:\test\adir\"
%unicode|string%(0) ""
%unicode|string%(13) "c:\test\adir\"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(13) "c:\test\adir\"
  [%u|b%"filename"]=>
  %unicode|string%(13) "c:\test\adir\"
}
-- Iteration 9 --
%unicode|string%(12) "/usr/include"
%unicode|string%(4) "arpa"
%unicode|string%(0) ""
%unicode|string%(4) "arpa"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(12) "/usr/include"
  [%u|b%"basename"]=>
  %unicode|string%(4) "arpa"
  [%u|b%"filename"]=>
  %unicode|string%(4) "arpa"
}
-- Iteration 10 --
%unicode|string%(12) "/usr/include"
%unicode|string%(4) "arpa"
%unicode|string%(0) ""
%unicode|string%(4) "arpa"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(12) "/usr/include"
  [%u|b%"basename"]=>
  %unicode|string%(4) "arpa"
  [%u|b%"filename"]=>
  %unicode|string%(4) "arpa"
}
-- Iteration 11 --
%unicode|string%(11) "usr/include"
%unicode|string%(4) "arpa"
%unicode|string%(0) ""
%unicode|string%(4) "arpa"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(11) "usr/include"
  [%u|b%"basename"]=>
  %unicode|string%(4) "arpa"
  [%u|b%"filename"]=>
  %unicode|string%(4) "arpa"
}
-- Iteration 12 --
%unicode|string%(11) "usr/include"
%unicode|string%(4) "arpa"
%unicode|string%(0) ""
%unicode|string%(4) "arpa"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(11) "usr/include"
  [%u|b%"basename"]=>
  %unicode|string%(4) "arpa"
  [%u|b%"filename"]=>
  %unicode|string%(4) "arpa"
}
-- Iteration 13 --
%unicode|string%(1) "."
%unicode|string%(13) "c:\test\afile"
%unicode|string%(0) ""
%unicode|string%(13) "c:\test\afile"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(13) "c:\test\afile"
  [%u|b%"filename"]=>
  %unicode|string%(13) "c:\test\afile"
}
-- Iteration 14 --
%unicode|string%(1) "."
%unicode|string%(13) "c:\test\afile"
%unicode|string%(0) ""
%unicode|string%(13) "c:\test\afile"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(13) "c:\test\afile"
  [%u|b%"filename"]=>
  %unicode|string%(13) "c:\test\afile"
}
-- Iteration 15 --
%unicode|string%(8) "c://test"
%unicode|string%(5) "afile"
%unicode|string%(0) ""
%unicode|string%(5) "afile"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(8) "c://test"
  [%u|b%"basename"]=>
  %unicode|string%(5) "afile"
  [%u|b%"filename"]=>
  %unicode|string%(5) "afile"
}
-- Iteration 16 --
%unicode|string%(1) "."
%unicode|string%(14) "c:\test\afile\"
%unicode|string%(0) ""
%unicode|string%(14) "c:\test\afile\"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(14) "c:\test\afile\"
  [%u|b%"filename"]=>
  %unicode|string%(14) "c:\test\afile\"
}
-- Iteration 17 --
%unicode|string%(1) "."
%unicode|string%(16) "c:\test\prog.exe"
%unicode|string%(3) "exe"
%unicode|string%(12) "c:\test\prog"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(16) "c:\test\prog.exe"
  [%u|b%"extension"]=>
  %unicode|string%(3) "exe"
  [%u|b%"filename"]=>
  %unicode|string%(12) "c:\test\prog"
}
-- Iteration 18 --
%unicode|string%(1) "."
%unicode|string%(16) "c:\test\prog.exe"
%unicode|string%(3) "exe"
%unicode|string%(12) "c:\test\prog"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(16) "c:\test\prog.exe"
  [%u|b%"extension"]=>
  %unicode|string%(3) "exe"
  [%u|b%"filename"]=>
  %unicode|string%(12) "c:\test\prog"
}
-- Iteration 19 --
%unicode|string%(7) "c:/test"
%unicode|string%(8) "prog.exe"
%unicode|string%(3) "exe"
%unicode|string%(4) "prog"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(7) "c:/test"
  [%u|b%"basename"]=>
  %unicode|string%(8) "prog.exe"
  [%u|b%"extension"]=>
  %unicode|string%(3) "exe"
  [%u|b%"filename"]=>
  %unicode|string%(4) "prog"
}
-- Iteration 20 --
%unicode|string%(17) "/usr/include/arpa"
%unicode|string%(6) "inet.h"
%unicode|string%(1) "h"
%unicode|string%(4) "inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(17) "/usr/include/arpa"
  [%u|b%"basename"]=>
  %unicode|string%(6) "inet.h"
  [%u|b%"extension"]=>
  %unicode|string%(1) "h"
  [%u|b%"filename"]=>
  %unicode|string%(4) "inet"
}
-- Iteration 21 --
%unicode|string%(19) "//usr/include//arpa"
%unicode|string%(6) "inet.h"
%unicode|string%(1) "h"
%unicode|string%(4) "inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(19) "//usr/include//arpa"
  [%u|b%"basename"]=>
  %unicode|string%(6) "inet.h"
  [%u|b%"extension"]=>
  %unicode|string%(1) "h"
  [%u|b%"filename"]=>
  %unicode|string%(4) "inet"
}
-- Iteration 22 --
%unicode|string%(1) "."
%unicode|string%(1) "\"
%unicode|string%(0) ""
%unicode|string%(1) "\"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(1) "\"
  [%u|b%"filename"]=>
  %unicode|string%(1) "\"
}
-- Iteration 23 --
%unicode|string%(1) "."
%unicode|string%(2) "\\"
%unicode|string%(0) ""
%unicode|string%(2) "\\"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(2) "\\"
  [%u|b%"filename"]=>
  %unicode|string%(2) "\\"
}
-- Iteration 24 --
%unicode|string%(1) "/"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "/"
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
-- Iteration 25 --
%unicode|string%(1) "/"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "/"
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
-- Iteration 26 --
%unicode|string%(1) "/"
%unicode|string%(0) ""
%unicode|string%(0) ""
%unicode|string%(0) ""
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "/"
  [%u|b%"basename"]=>
  %unicode|string%(0) ""
  [%u|b%"filename"]=>
  %unicode|string%(0) ""
}
-- Iteration 27 --
%unicode|string%(17) "/usr/include/arpa"
%unicode|string%(6) "inet.h"
%unicode|string%(1) "h"
%unicode|string%(4) "inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(17) "/usr/include/arpa"
  [%u|b%"basename"]=>
  %unicode|string%(6) "inet.h"
  [%u|b%"extension"]=>
  %unicode|string%(1) "h"
  [%u|b%"filename"]=>
  %unicode|string%(4) "inet"
}
-- Iteration 28 --
%unicode|string%(27) "c:\windows/system32\drivers"
%unicode|string%(9) "etc\hosts"
%unicode|string%(0) ""
%unicode|string%(9) "etc\hosts"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(27) "c:\windows/system32\drivers"
  [%u|b%"basename"]=>
  %unicode|string%(9) "etc\hosts"
  [%u|b%"filename"]=>
  %unicode|string%(9) "etc\hosts"
}
-- Iteration 29 --
%unicode|string%(12) "/usr\include"
%unicode|string%(11) "arpa\inet.h"
%unicode|string%(1) "h"
%unicode|string%(9) "arpa\inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(12) "/usr\include"
  [%u|b%"basename"]=>
  %unicode|string%(11) "arpa\inet.h"
  [%u|b%"extension"]=>
  %unicode|string%(1) "h"
  [%u|b%"filename"]=>
  %unicode|string%(9) "arpa\inet"
}
-- Iteration 30 --
%unicode|string%(1) "."
%unicode|string%(25) "   c:\test\adir\afile.txt"
%unicode|string%(3) "txt"
%unicode|string%(21) "   c:\test\adir\afile"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(25) "   c:\test\adir\afile.txt"
  [%u|b%"extension"]=>
  %unicode|string%(3) "txt"
  [%u|b%"filename"]=>
  %unicode|string%(21) "   c:\test\adir\afile"
}
-- Iteration 31 --
%unicode|string%(1) "."
%unicode|string%(25) "c:\test\adir\afile.txt   "
%unicode|string%(6) "txt   "
%unicode|string%(18) "c:\test\adir\afile"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(25) "c:\test\adir\afile.txt   "
  [%u|b%"extension"]=>
  %unicode|string%(6) "txt   "
  [%u|b%"filename"]=>
  %unicode|string%(18) "c:\test\adir\afile"
}
-- Iteration 32 --
%unicode|string%(1) "."
%unicode|string%(28) "   c:\test\adir\afile.txt   "
%unicode|string%(6) "txt   "
%unicode|string%(21) "   c:\test\adir\afile"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(28) "   c:\test\adir\afile.txt   "
  [%u|b%"extension"]=>
  %unicode|string%(6) "txt   "
  [%u|b%"filename"]=>
  %unicode|string%(21) "   c:\test\adir\afile"
}
-- Iteration 33 --
%unicode|string%(20) "   /usr/include/arpa"
%unicode|string%(6) "inet.h"
%unicode|string%(1) "h"
%unicode|string%(4) "inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(20) "   /usr/include/arpa"
  [%u|b%"basename"]=>
  %unicode|string%(6) "inet.h"
  [%u|b%"extension"]=>
  %unicode|string%(1) "h"
  [%u|b%"filename"]=>
  %unicode|string%(4) "inet"
}
-- Iteration 34 --
%unicode|string%(17) "/usr/include/arpa"
%unicode|string%(9) "inet.h   "
%unicode|string%(4) "h   "
%unicode|string%(4) "inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(17) "/usr/include/arpa"
  [%u|b%"basename"]=>
  %unicode|string%(9) "inet.h   "
  [%u|b%"extension"]=>
  %unicode|string%(4) "h   "
  [%u|b%"filename"]=>
  %unicode|string%(4) "inet"
}
-- Iteration 35 --
%unicode|string%(20) "   /usr/include/arpa"
%unicode|string%(9) "inet.h   "
%unicode|string%(4) "h   "
%unicode|string%(4) "inet"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(20) "   /usr/include/arpa"
  [%u|b%"basename"]=>
  %unicode|string%(9) "inet.h   "
  [%u|b%"extension"]=>
  %unicode|string%(4) "h   "
  [%u|b%"filename"]=>
  %unicode|string%(4) "inet"
}
-- Iteration 36 --
%unicode|string%(1) "."
%unicode|string%(3) " c:"
%unicode|string%(0) ""
%unicode|string%(3) " c:"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(3) " c:"
  [%u|b%"filename"]=>
  %unicode|string%(3) " c:"
}
-- Iteration 37 --
%unicode|string%(1) "."
%unicode|string%(24) "		c:\test\adir\afile.txt"
%unicode|string%(3) "txt"
%unicode|string%(20) "		c:\test\adir\afile"
array(4) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "."
  [%u|b%"basename"]=>
  %unicode|string%(24) "		c:\test\adir\afile.txt"
  [%u|b%"extension"]=>
  %unicode|string%(3) "txt"
  [%u|b%"filename"]=>
  %unicode|string%(20) "		c:\test\adir\afile"
}
-- Iteration 38 --
%unicode|string%(1) "/"
%unicode|string%(3) "usr"
%unicode|string%(0) ""
%unicode|string%(3) "usr"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "/"
  [%u|b%"basename"]=>
  %unicode|string%(3) "usr"
  [%u|b%"filename"]=>
  %unicode|string%(3) "usr"
}
-- Iteration 39 --
%unicode|string%(1) "/"
%unicode|string%(3) "usr"
%unicode|string%(0) ""
%unicode|string%(3) "usr"
array(3) {
  [%u|b%"dirname"]=>
  %unicode|string%(1) "/"
  [%u|b%"basename"]=>
  %unicode|string%(3) "usr"
  [%u|b%"filename"]=>
  %unicode|string%(3) "usr"
}
Done

