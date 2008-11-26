--TEST--
Test dirname() function : basic functionality 
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Not valid for Windows");
?>
--FILE--
<?php
/* Prototype  : string dirname(string path)
 * Description: Returns the directory name component of the path 
 * Source code: ext/standard/string.c
 * Alias to functions: 
 */

echo "*** Testing dirname() : basic functionality ***\n";


// Initialise all required variables
$paths = array(
 			'',
 			' ',
			'c:',
			'c:\\',
			'c:/',
			'afile',
			'c:\test\afile',
			'c:\\test\\afile',
			'c://test//afile',
			'c:\test\afile\\',
			'/usr/lib/locale/en_US',
			'//usr/lib//locale/en_US',
			'\\',
			'\\\\',
			'/',
			'//',
			'///',
			'/usr/lib/locale/en_US/',
			'c:\windows/system32\drivers/etc\hosts',
			'/usr\lib/locale\en_US',
			'   c:\test\adir\afile.txt',
			'c:\test\adir\afile.txt   ',
			'   c:\test\adir\afile.txt   ',
			'   /usr/lib/locale/en_US',
			'/usr/lib/locale/en_US   ',
			'   /usr/lib/locale/en_US   ',
			' c:',
			'		c:\test\adir\afile.txt',
			'/usr',
			'/usr/'			
			);

foreach ($paths as $path) {
	var_dump( dirname($path) );
}

?>
===DONE===
--EXPECTF--
*** Testing dirname() : basic functionality ***
string(0) ""
string(1) "."
string(1) "."
string(1) "."
string(1) "."
string(1) "."
string(1) "."
string(1) "."
string(8) "c://test"
string(1) "."
string(15) "/usr/lib/locale"
string(17) "//usr/lib//locale"
string(1) "."
string(1) "."
string(1) "/"
string(1) "/"
string(1) "/"
string(15) "/usr/lib/locale"
string(27) "c:\windows/system32\drivers"
string(8) "/usr\lib"
string(1) "."
string(1) "."
string(1) "."
string(18) "   /usr/lib/locale"
string(15) "/usr/lib/locale"
string(18) "   /usr/lib/locale"
string(1) "."
string(1) "."
string(1) "/"
string(1) "/"
===DONE===

