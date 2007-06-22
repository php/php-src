--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
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
$filename = dirname(__FILE__)."/symlink_link_linkinfo_is_link.tmp";
$fp = fopen($filename, "w");  // create temp file
fclose($fp);

// linkname used to create soft/hard link
$linkname = dirname(__FILE__)."/symlink_link_linkinfo_is_link_link.tmp";

echo "*** Testing symlink() for error conditions ***\n";
//zero arguments
var_dump( symlink() );

//more than expected
var_dump( symlink($filename, $linkname, true) );

//invalid arguments
var_dump( symlink(NULL, $linkname) );  // NULL as filename
var_dump( symlink('', $linkname) );  // empty string as filename
var_dump( symlink(false, $linkname) );  // boolean false as filename
var_dump( symlink($filename, NULL) );  // NULL as linkname
var_dump( symlink($filename, '') );  // '' as linkname
var_dump( symlink($filename, false) );  // false as linkname

echo "\n*** Testing linkinfo() for error conditions ***\n";
//zero arguments
var_dump( linkinfo() );

//more than expected
var_dump( linkinfo($linkname, true) );

//invalid arguments
var_dump( linkinfo(NULL) );  // NULL as linkname
var_dump( linkinfo('') );  // empty string as linkname
var_dump( linkinfo(false) );  // boolean false as linkname

echo "\n*** Testing link() for error conditions ***\n";
//zero arguments
var_dump( link() );

//more than expected
var_dump( link($filename, $linkname, false) );

//invalid arguments
var_dump( link(NULL, $linkname) );  // NULL as filename
var_dump( link('', $linkname) );  // empty string as filename
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
var_dump( is_link(false) );  // boolean false as linkname
var_dump( is_link($filename) );  // file given to is_link

echo "Done\n";
?>
--CLEAN--
<?php
unlink(dirname(__FILE__)."/symlink_link_linkinfo_is_link.tmp");
?>
--EXPECTF--
*** Testing symlink() for error conditions ***

Warning: symlink() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: symlink() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

*** Testing linkinfo() for error conditions ***

Warning: linkinfo() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: linkinfo() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)

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

*** Testing is_link() for error conditions ***

Warning: is_link() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: is_link() expects exactly 1 parameter, 2 given in %s on line %d
NULL
bool(false)
bool(false)
bool(false)
bool(false)
Done
--UEXPECTF--
*** Testing symlink() for error conditions ***

Warning: symlink() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: symlink() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

Warning: symlink(): No such file or directory in %s on line %d
bool(false)

*** Testing linkinfo() for error conditions ***

Warning: linkinfo() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: linkinfo() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)

Warning: linkinfo(): No such file or directory in %s on line %d
int(-1)

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

*** Testing is_link() for error conditions ***

Warning: is_link() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: is_link() expects exactly 1 parameter, 2 given in %s on line %d
NULL
bool(false)
bool(false)
bool(false)
bool(false)
Done
