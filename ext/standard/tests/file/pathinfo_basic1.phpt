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
--EXPECT--
*** Testing basic functions of pathinfo() ***
-- Iteration 1 --
string(0) ""
unicode(0) ""
string(0) ""
unicode(0) ""
array(2) {
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
-- Iteration 2 --
unicode(1) "."
unicode(1) " "
string(0) ""
unicode(1) " "
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(1) " "
  [u"filename"]=>
  unicode(1) " "
}
-- Iteration 3 --
unicode(1) "."
unicode(2) "c:"
string(0) ""
unicode(2) "c:"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(2) "c:"
  [u"filename"]=>
  unicode(2) "c:"
}
-- Iteration 4 --
unicode(1) "."
unicode(3) "c:\"
string(0) ""
unicode(3) "c:\"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(3) "c:\"
  [u"filename"]=>
  unicode(3) "c:\"
}
-- Iteration 5 --
unicode(1) "."
unicode(2) "c:"
string(0) ""
unicode(2) "c:"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(2) "c:"
  [u"filename"]=>
  unicode(2) "c:"
}
-- Iteration 6 --
unicode(1) "."
unicode(5) "afile"
string(0) ""
unicode(5) "afile"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(5) "afile"
  [u"filename"]=>
  unicode(5) "afile"
}
-- Iteration 7 --
unicode(1) "."
unicode(12) "c:\test\adir"
string(0) ""
unicode(12) "c:\test\adir"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(12) "c:\test\adir"
  [u"filename"]=>
  unicode(12) "c:\test\adir"
}
-- Iteration 8 --
unicode(1) "."
unicode(13) "c:\test\adir\"
string(0) ""
unicode(13) "c:\test\adir\"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(13) "c:\test\adir\"
  [u"filename"]=>
  unicode(13) "c:\test\adir\"
}
-- Iteration 9 --
unicode(12) "/usr/include"
unicode(4) "arpa"
string(0) ""
unicode(4) "arpa"
array(3) {
  [u"dirname"]=>
  unicode(12) "/usr/include"
  [u"basename"]=>
  unicode(4) "arpa"
  [u"filename"]=>
  unicode(4) "arpa"
}
-- Iteration 10 --
unicode(12) "/usr/include"
unicode(4) "arpa"
string(0) ""
unicode(4) "arpa"
array(3) {
  [u"dirname"]=>
  unicode(12) "/usr/include"
  [u"basename"]=>
  unicode(4) "arpa"
  [u"filename"]=>
  unicode(4) "arpa"
}
-- Iteration 11 --
unicode(11) "usr/include"
unicode(4) "arpa"
string(0) ""
unicode(4) "arpa"
array(3) {
  [u"dirname"]=>
  unicode(11) "usr/include"
  [u"basename"]=>
  unicode(4) "arpa"
  [u"filename"]=>
  unicode(4) "arpa"
}
-- Iteration 12 --
unicode(11) "usr/include"
unicode(4) "arpa"
string(0) ""
unicode(4) "arpa"
array(3) {
  [u"dirname"]=>
  unicode(11) "usr/include"
  [u"basename"]=>
  unicode(4) "arpa"
  [u"filename"]=>
  unicode(4) "arpa"
}
-- Iteration 13 --
unicode(1) "."
unicode(13) "c:\test\afile"
string(0) ""
unicode(13) "c:\test\afile"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(13) "c:\test\afile"
  [u"filename"]=>
  unicode(13) "c:\test\afile"
}
-- Iteration 14 --
unicode(1) "."
unicode(13) "c:\test\afile"
string(0) ""
unicode(13) "c:\test\afile"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(13) "c:\test\afile"
  [u"filename"]=>
  unicode(13) "c:\test\afile"
}
-- Iteration 15 --
unicode(8) "c://test"
unicode(5) "afile"
string(0) ""
unicode(5) "afile"
array(3) {
  [u"dirname"]=>
  unicode(8) "c://test"
  [u"basename"]=>
  unicode(5) "afile"
  [u"filename"]=>
  unicode(5) "afile"
}
-- Iteration 16 --
unicode(1) "."
unicode(14) "c:\test\afile\"
string(0) ""
unicode(14) "c:\test\afile\"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(14) "c:\test\afile\"
  [u"filename"]=>
  unicode(14) "c:\test\afile\"
}
-- Iteration 17 --
unicode(1) "."
unicode(16) "c:\test\prog.exe"
unicode(3) "exe"
unicode(12) "c:\test\prog"
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(16) "c:\test\prog.exe"
  [u"extension"]=>
  unicode(3) "exe"
  [u"filename"]=>
  unicode(12) "c:\test\prog"
}
-- Iteration 18 --
unicode(1) "."
unicode(16) "c:\test\prog.exe"
unicode(3) "exe"
unicode(12) "c:\test\prog"
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(16) "c:\test\prog.exe"
  [u"extension"]=>
  unicode(3) "exe"
  [u"filename"]=>
  unicode(12) "c:\test\prog"
}
-- Iteration 19 --
unicode(7) "c:/test"
unicode(8) "prog.exe"
unicode(3) "exe"
unicode(4) "prog"
array(4) {
  [u"dirname"]=>
  unicode(7) "c:/test"
  [u"basename"]=>
  unicode(8) "prog.exe"
  [u"extension"]=>
  unicode(3) "exe"
  [u"filename"]=>
  unicode(4) "prog"
}
-- Iteration 20 --
unicode(17) "/usr/include/arpa"
unicode(6) "inet.h"
unicode(1) "h"
unicode(4) "inet"
array(4) {
  [u"dirname"]=>
  unicode(17) "/usr/include/arpa"
  [u"basename"]=>
  unicode(6) "inet.h"
  [u"extension"]=>
  unicode(1) "h"
  [u"filename"]=>
  unicode(4) "inet"
}
-- Iteration 21 --
unicode(19) "//usr/include//arpa"
unicode(6) "inet.h"
unicode(1) "h"
unicode(4) "inet"
array(4) {
  [u"dirname"]=>
  unicode(19) "//usr/include//arpa"
  [u"basename"]=>
  unicode(6) "inet.h"
  [u"extension"]=>
  unicode(1) "h"
  [u"filename"]=>
  unicode(4) "inet"
}
-- Iteration 22 --
unicode(1) "."
unicode(1) "\"
string(0) ""
unicode(1) "\"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(1) "\"
  [u"filename"]=>
  unicode(1) "\"
}
-- Iteration 23 --
unicode(1) "."
unicode(2) "\\"
string(0) ""
unicode(2) "\\"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(2) "\\"
  [u"filename"]=>
  unicode(2) "\\"
}
-- Iteration 24 --
unicode(1) "/"
unicode(0) ""
string(0) ""
unicode(0) ""
array(3) {
  [u"dirname"]=>
  unicode(1) "/"
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
-- Iteration 25 --
unicode(1) "/"
unicode(0) ""
string(0) ""
unicode(0) ""
array(3) {
  [u"dirname"]=>
  unicode(1) "/"
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
-- Iteration 26 --
unicode(1) "/"
unicode(0) ""
string(0) ""
unicode(0) ""
array(3) {
  [u"dirname"]=>
  unicode(1) "/"
  [u"basename"]=>
  unicode(0) ""
  [u"filename"]=>
  unicode(0) ""
}
-- Iteration 27 --
unicode(17) "/usr/include/arpa"
unicode(6) "inet.h"
unicode(1) "h"
unicode(4) "inet"
array(4) {
  [u"dirname"]=>
  unicode(17) "/usr/include/arpa"
  [u"basename"]=>
  unicode(6) "inet.h"
  [u"extension"]=>
  unicode(1) "h"
  [u"filename"]=>
  unicode(4) "inet"
}
-- Iteration 28 --
unicode(27) "c:\windows/system32\drivers"
unicode(9) "etc\hosts"
string(0) ""
unicode(9) "etc\hosts"
array(3) {
  [u"dirname"]=>
  unicode(27) "c:\windows/system32\drivers"
  [u"basename"]=>
  unicode(9) "etc\hosts"
  [u"filename"]=>
  unicode(9) "etc\hosts"
}
-- Iteration 29 --
unicode(12) "/usr\include"
unicode(11) "arpa\inet.h"
unicode(1) "h"
unicode(9) "arpa\inet"
array(4) {
  [u"dirname"]=>
  unicode(12) "/usr\include"
  [u"basename"]=>
  unicode(11) "arpa\inet.h"
  [u"extension"]=>
  unicode(1) "h"
  [u"filename"]=>
  unicode(9) "arpa\inet"
}
-- Iteration 30 --
unicode(1) "."
unicode(25) "   c:\test\adir\afile.txt"
unicode(3) "txt"
unicode(21) "   c:\test\adir\afile"
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(25) "   c:\test\adir\afile.txt"
  [u"extension"]=>
  unicode(3) "txt"
  [u"filename"]=>
  unicode(21) "   c:\test\adir\afile"
}
-- Iteration 31 --
unicode(1) "."
unicode(25) "c:\test\adir\afile.txt   "
unicode(6) "txt   "
unicode(18) "c:\test\adir\afile"
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(25) "c:\test\adir\afile.txt   "
  [u"extension"]=>
  unicode(6) "txt   "
  [u"filename"]=>
  unicode(18) "c:\test\adir\afile"
}
-- Iteration 32 --
unicode(1) "."
unicode(28) "   c:\test\adir\afile.txt   "
unicode(6) "txt   "
unicode(21) "   c:\test\adir\afile"
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(28) "   c:\test\adir\afile.txt   "
  [u"extension"]=>
  unicode(6) "txt   "
  [u"filename"]=>
  unicode(21) "   c:\test\adir\afile"
}
-- Iteration 33 --
unicode(20) "   /usr/include/arpa"
unicode(6) "inet.h"
unicode(1) "h"
unicode(4) "inet"
array(4) {
  [u"dirname"]=>
  unicode(20) "   /usr/include/arpa"
  [u"basename"]=>
  unicode(6) "inet.h"
  [u"extension"]=>
  unicode(1) "h"
  [u"filename"]=>
  unicode(4) "inet"
}
-- Iteration 34 --
unicode(17) "/usr/include/arpa"
unicode(9) "inet.h   "
unicode(4) "h   "
unicode(4) "inet"
array(4) {
  [u"dirname"]=>
  unicode(17) "/usr/include/arpa"
  [u"basename"]=>
  unicode(9) "inet.h   "
  [u"extension"]=>
  unicode(4) "h   "
  [u"filename"]=>
  unicode(4) "inet"
}
-- Iteration 35 --
unicode(20) "   /usr/include/arpa"
unicode(9) "inet.h   "
unicode(4) "h   "
unicode(4) "inet"
array(4) {
  [u"dirname"]=>
  unicode(20) "   /usr/include/arpa"
  [u"basename"]=>
  unicode(9) "inet.h   "
  [u"extension"]=>
  unicode(4) "h   "
  [u"filename"]=>
  unicode(4) "inet"
}
-- Iteration 36 --
unicode(1) "."
unicode(3) " c:"
string(0) ""
unicode(3) " c:"
array(3) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(3) " c:"
  [u"filename"]=>
  unicode(3) " c:"
}
-- Iteration 37 --
unicode(1) "."
unicode(24) "		c:\test\adir\afile.txt"
unicode(3) "txt"
unicode(20) "		c:\test\adir\afile"
array(4) {
  [u"dirname"]=>
  unicode(1) "."
  [u"basename"]=>
  unicode(24) "		c:\test\adir\afile.txt"
  [u"extension"]=>
  unicode(3) "txt"
  [u"filename"]=>
  unicode(20) "		c:\test\adir\afile"
}
-- Iteration 38 --
unicode(1) "/"
unicode(3) "usr"
string(0) ""
unicode(3) "usr"
array(3) {
  [u"dirname"]=>
  unicode(1) "/"
  [u"basename"]=>
  unicode(3) "usr"
  [u"filename"]=>
  unicode(3) "usr"
}
-- Iteration 39 --
unicode(1) "/"
unicode(3) "usr"
string(0) ""
unicode(3) "usr"
array(3) {
  [u"dirname"]=>
  unicode(1) "/"
  [u"basename"]=>
  unicode(3) "usr"
  [u"filename"]=>
  unicode(3) "usr"
}
Done

