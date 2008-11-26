--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : error conditions - link & is_link
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
if (PHP_INT_SIZE != 4) {
  die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php
/* Prototype: bool symlink ( string $target, string $link );
   Description: creates a symbolic link to the existing target with the specified name link

   Prototype: bool is_link ( string $filename );
   Description: Tells whether the given file is a symbolic link.

   Prototype: bool link ( string $target, string $link );
   Description: Create a hard link

   Prototype: int linkinfo ( string $path );
   Description: Gets information about a link
*/

// create temp $filename and create link $linkname to it
$filename = dirname(__FILE__)."/symlink_link_linkinfo_is_link_error2.tmp";
$fp = fopen($filename, "w");  // create temp file
fclose($fp);

// linkname used to create soft/hard link
$linkname = dirname(__FILE__)."/symlink_link_linkinfo_is_link_link_error2.tmp";

echo "*** Testing link() for error conditions ***\n";
//zero arguments
var_dump( link() );

//more than expected
var_dump( link($filename, $linkname, false) );

//invalid arguments
var_dump( link(NULL, $linkname) );  // NULL as filename
var_dump( link('', $linkname) );  // empty string as filename
var_dump( link(' ', $linkname) );  // space as filename
var_dump( link(false, $linkname) );  // boolean false as filename
var_dump( link($filename, NULL) );  // NULL as linkname
var_dump( link($filename, '') );  // '' as linkname
var_dump( link($filename, false) );  // false as linkname

echo "\n*** Testing is_link() for error conditions ***\n";
//zero arguments
var_dump( is_link() );

//more than expected
var_dump( is_link($linkname, "/") );

//invalid arguments
var_dump( is_link(NULL) );  // NULL as linkname
var_dump( is_link('') );  // empty string as linkname
var_dump( is_link(' ') );  // space as linkname
var_dump( is_link(false) );  // boolean false as linkname
var_dump( is_link($filename) );  // file given to is_link

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/symlink_link_linkinfo_is_link_error2.tmp");
?>
--EXPECTF--
*** Testing link() for error conditions ***

Warning: link() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: link() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: link(): No such file or directory in %s on line %d
bool(false)

Warning: link(): No such file or directory in %s on line %d
bool(false)

*** Testing is_link() for error conditions ***

Warning: is_link() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: is_link() expects exactly 1 parameter, 2 given in %s on line %d
NULL
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done

