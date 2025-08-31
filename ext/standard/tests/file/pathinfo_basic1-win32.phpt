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
string(0) ""
string(0) ""
string(0) ""
array(2) {
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
-- Iteration 2 --
string(1) "."
string(1) " "
string(0) ""
string(1) " "
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(1) " "
  ["filename"]=>
  string(1) " "
}
-- Iteration 3 --
string(2) "c:"
string(1) "c"
string(0) ""
string(1) "c"
array(3) {
  ["dirname"]=>
  string(2) "c:"
  ["basename"]=>
  string(1) "c"
  ["filename"]=>
  string(1) "c"
}
-- Iteration 4 --
string(3) "c:\"
string(1) "c"
string(0) ""
string(1) "c"
array(3) {
  ["dirname"]=>
  string(3) "c:\"
  ["basename"]=>
  string(1) "c"
  ["filename"]=>
  string(1) "c"
}
-- Iteration 5 --
string(3) "c:\"
string(1) "c"
string(0) ""
string(1) "c"
array(3) {
  ["dirname"]=>
  string(3) "c:\"
  ["basename"]=>
  string(1) "c"
  ["filename"]=>
  string(1) "c"
}
-- Iteration 6 --
string(1) "."
string(5) "afile"
string(0) ""
string(5) "afile"
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(5) "afile"
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 7 --
string(7) "c:\test"
string(4) "adir"
string(0) ""
string(4) "adir"
array(3) {
  ["dirname"]=>
  string(7) "c:\test"
  ["basename"]=>
  string(4) "adir"
  ["filename"]=>
  string(4) "adir"
}
-- Iteration 8 --
string(7) "c:\test"
string(4) "adir"
string(0) ""
string(4) "adir"
array(3) {
  ["dirname"]=>
  string(7) "c:\test"
  ["basename"]=>
  string(4) "adir"
  ["filename"]=>
  string(4) "adir"
}
-- Iteration 9 --
string(12) "/usr/include"
string(4) "arpa"
string(0) ""
string(4) "arpa"
array(3) {
  ["dirname"]=>
  string(12) "/usr/include"
  ["basename"]=>
  string(4) "arpa"
  ["filename"]=>
  string(4) "arpa"
}
-- Iteration 10 --
string(12) "/usr/include"
string(4) "arpa"
string(0) ""
string(4) "arpa"
array(3) {
  ["dirname"]=>
  string(12) "/usr/include"
  ["basename"]=>
  string(4) "arpa"
  ["filename"]=>
  string(4) "arpa"
}
-- Iteration 11 --
string(11) "usr/include"
string(4) "arpa"
string(0) ""
string(4) "arpa"
array(3) {
  ["dirname"]=>
  string(11) "usr/include"
  ["basename"]=>
  string(4) "arpa"
  ["filename"]=>
  string(4) "arpa"
}
-- Iteration 12 --
string(11) "usr/include"
string(4) "arpa"
string(0) ""
string(4) "arpa"
array(3) {
  ["dirname"]=>
  string(11) "usr/include"
  ["basename"]=>
  string(4) "arpa"
  ["filename"]=>
  string(4) "arpa"
}
-- Iteration 13 --
string(7) "c:\test"
string(5) "afile"
string(0) ""
string(5) "afile"
array(3) {
  ["dirname"]=>
  string(7) "c:\test"
  ["basename"]=>
  string(5) "afile"
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 14 --
string(7) "c:\test"
string(5) "afile"
string(0) ""
string(5) "afile"
array(3) {
  ["dirname"]=>
  string(7) "c:\test"
  ["basename"]=>
  string(5) "afile"
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 15 --
string(8) "c://test"
string(5) "afile"
string(0) ""
string(5) "afile"
array(3) {
  ["dirname"]=>
  string(8) "c://test"
  ["basename"]=>
  string(5) "afile"
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 16 --
string(7) "c:\test"
string(5) "afile"
string(0) ""
string(5) "afile"
array(3) {
  ["dirname"]=>
  string(7) "c:\test"
  ["basename"]=>
  string(5) "afile"
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 17 --
string(7) "c:\test"
string(8) "prog.exe"
string(3) "exe"
string(4) "prog"
array(4) {
  ["dirname"]=>
  string(7) "c:\test"
  ["basename"]=>
  string(8) "prog.exe"
  ["extension"]=>
  string(3) "exe"
  ["filename"]=>
  string(4) "prog"
}
-- Iteration 18 --
string(7) "c:\test"
string(8) "prog.exe"
string(3) "exe"
string(4) "prog"
array(4) {
  ["dirname"]=>
  string(7) "c:\test"
  ["basename"]=>
  string(8) "prog.exe"
  ["extension"]=>
  string(3) "exe"
  ["filename"]=>
  string(4) "prog"
}
-- Iteration 19 --
string(7) "c:/test"
string(8) "prog.exe"
string(3) "exe"
string(4) "prog"
array(4) {
  ["dirname"]=>
  string(7) "c:/test"
  ["basename"]=>
  string(8) "prog.exe"
  ["extension"]=>
  string(3) "exe"
  ["filename"]=>
  string(4) "prog"
}
-- Iteration 20 --
string(17) "/usr/include/arpa"
string(6) "inet.h"
string(1) "h"
string(4) "inet"
array(4) {
  ["dirname"]=>
  string(17) "/usr/include/arpa"
  ["basename"]=>
  string(6) "inet.h"
  ["extension"]=>
  string(1) "h"
  ["filename"]=>
  string(4) "inet"
}
-- Iteration 21 --
string(19) "//usr/include//arpa"
string(6) "inet.h"
string(1) "h"
string(4) "inet"
array(4) {
  ["dirname"]=>
  string(19) "//usr/include//arpa"
  ["basename"]=>
  string(6) "inet.h"
  ["extension"]=>
  string(1) "h"
  ["filename"]=>
  string(4) "inet"
}
-- Iteration 22 --
string(1) "\"
string(0) ""
string(0) ""
string(0) ""
array(3) {
  ["dirname"]=>
  string(1) "\"
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
-- Iteration 23 --
string(1) "\"
string(0) ""
string(0) ""
string(0) ""
array(3) {
  ["dirname"]=>
  string(1) "\"
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
-- Iteration 24 --
string(1) "\"
string(0) ""
string(0) ""
string(0) ""
array(3) {
  ["dirname"]=>
  string(1) "\"
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
-- Iteration 25 --
string(1) "\"
string(0) ""
string(0) ""
string(0) ""
array(3) {
  ["dirname"]=>
  string(1) "\"
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
-- Iteration 26 --
string(1) "\"
string(0) ""
string(0) ""
string(0) ""
array(3) {
  ["dirname"]=>
  string(1) "\"
  ["basename"]=>
  string(0) ""
  ["filename"]=>
  string(0) ""
}
-- Iteration 27 --
string(17) "/usr/include/arpa"
string(6) "inet.h"
string(1) "h"
string(4) "inet"
array(4) {
  ["dirname"]=>
  string(17) "/usr/include/arpa"
  ["basename"]=>
  string(6) "inet.h"
  ["extension"]=>
  string(1) "h"
  ["filename"]=>
  string(4) "inet"
}
-- Iteration 28 --
string(31) "c:\windows/system32\drivers/etc"
string(5) "hosts"
string(0) ""
string(5) "hosts"
array(3) {
  ["dirname"]=>
  string(31) "c:\windows/system32\drivers/etc"
  ["basename"]=>
  string(5) "hosts"
  ["filename"]=>
  string(5) "hosts"
}
-- Iteration 29 --
string(17) "/usr\include/arpa"
string(6) "inet.h"
string(1) "h"
string(4) "inet"
array(4) {
  ["dirname"]=>
  string(17) "/usr\include/arpa"
  ["basename"]=>
  string(6) "inet.h"
  ["extension"]=>
  string(1) "h"
  ["filename"]=>
  string(4) "inet"
}
-- Iteration 30 --
string(15) "   c:\test\adir"
string(9) "afile.txt"
string(3) "txt"
string(5) "afile"
array(4) {
  ["dirname"]=>
  string(15) "   c:\test\adir"
  ["basename"]=>
  string(9) "afile.txt"
  ["extension"]=>
  string(3) "txt"
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 31 --
string(12) "c:\test\adir"
string(12) "afile.txt   "
string(6) "txt   "
string(5) "afile"
array(4) {
  ["dirname"]=>
  string(12) "c:\test\adir"
  ["basename"]=>
  string(12) "afile.txt   "
  ["extension"]=>
  string(6) "txt   "
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 32 --
string(15) "   c:\test\adir"
string(12) "afile.txt   "
string(6) "txt   "
string(5) "afile"
array(4) {
  ["dirname"]=>
  string(15) "   c:\test\adir"
  ["basename"]=>
  string(12) "afile.txt   "
  ["extension"]=>
  string(6) "txt   "
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 33 --
string(20) "   /usr/include/arpa"
string(6) "inet.h"
string(1) "h"
string(4) "inet"
array(4) {
  ["dirname"]=>
  string(20) "   /usr/include/arpa"
  ["basename"]=>
  string(6) "inet.h"
  ["extension"]=>
  string(1) "h"
  ["filename"]=>
  string(4) "inet"
}
-- Iteration 34 --
string(17) "/usr/include/arpa"
string(9) "inet.h   "
string(4) "h   "
string(4) "inet"
array(4) {
  ["dirname"]=>
  string(17) "/usr/include/arpa"
  ["basename"]=>
  string(9) "inet.h   "
  ["extension"]=>
  string(4) "h   "
  ["filename"]=>
  string(4) "inet"
}
-- Iteration 35 --
string(20) "   /usr/include/arpa"
string(9) "inet.h   "
string(4) "h   "
string(4) "inet"
array(4) {
  ["dirname"]=>
  string(20) "   /usr/include/arpa"
  ["basename"]=>
  string(9) "inet.h   "
  ["extension"]=>
  string(4) "h   "
  ["filename"]=>
  string(4) "inet"
}
-- Iteration 36 --
string(1) "."
string(3) " c:"
string(0) ""
string(3) " c:"
array(3) {
  ["dirname"]=>
  string(1) "."
  ["basename"]=>
  string(3) " c:"
  ["filename"]=>
  string(3) " c:"
}
-- Iteration 37 --
string(14) "		c:\test\adir"
string(9) "afile.txt"
string(3) "txt"
string(5) "afile"
array(4) {
  ["dirname"]=>
  string(14) "		c:\test\adir"
  ["basename"]=>
  string(9) "afile.txt"
  ["extension"]=>
  string(3) "txt"
  ["filename"]=>
  string(5) "afile"
}
-- Iteration 38 --
string(1) "\"
string(3) "usr"
string(0) ""
string(3) "usr"
array(3) {
  ["dirname"]=>
  string(1) "\"
  ["basename"]=>
  string(3) "usr"
  ["filename"]=>
  string(3) "usr"
}
-- Iteration 39 --
string(1) "\"
string(3) "usr"
string(0) ""
string(3) "usr"
array(3) {
  ["dirname"]=>
  string(1) "\"
  ["basename"]=>
  string(3) "usr"
  ["filename"]=>
  string(3) "usr"
}
Done
