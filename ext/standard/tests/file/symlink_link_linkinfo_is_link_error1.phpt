--TEST--
Test symlink(), linkinfo(), link() and is_link() functions : error conditions - symlink & linkinfo
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
if (substr(PHP_OS, 0, 3) == 'SUN') {
  die('skip Not valid for Sun Solaris');
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
$filename = __DIR__."/symlink_link_linkinfo_is_link_error1.tmp";
$fp = fopen($filename, "w");  // create temp file
fclose($fp);

// linkname used to create soft/hard link
$linkname = __DIR__."/symlink_link_linkinfo_is_link_link_error1.tmp";

echo "*** Testing symlink() for error conditions ***\n";

//invalid arguments
var_dump( symlink(NULL, $linkname) );  // NULL as filename
var_dump( symlink('', $linkname) );  // empty string as filename
var_dump( symlink(false, $linkname) );  // boolean false as filename
var_dump( symlink($filename, NULL) );  // NULL as linkname
var_dump( symlink($filename, '') );  // '' as linkname
var_dump( symlink($filename, false) );  // false as linkname

echo "\n*** Testing linkinfo() for error conditions ***\n";

//invalid arguments
var_dump( linkinfo(NULL) );  // NULL as linkname
var_dump( linkinfo('') );  // empty string as linkname
var_dump( linkinfo(false) );  // boolean false as linkname

echo "Done\n";
?>
--CLEAN--
<?php
unlink(__DIR__."/symlink_link_linkinfo_is_link_error1.tmp");
@unlink(__DIR__."/symlink_link_linkinfo_is_link_link_error1.tmp");
?>
--EXPECTF--
*** Testing symlink() for error conditions ***

Warning: symlink(): %s in %s on line %d
bool(false)

Warning: symlink(): %s in %s on line %d
bool(false)

Warning: symlink(): %s in %s on line %d
bool(false)

Warning: symlink(): %s in %s on line %d
bool(false)

Warning: symlink(): %s in %s on line %d
bool(false)

Warning: symlink(): %s in %s on line %d
bool(false)

*** Testing linkinfo() for error conditions ***

Warning: linkinfo(): %s in %s on line %d
int(-1)

Warning: linkinfo(): %s in %s on line %d
int(-1)

Warning: linkinfo(): %s in %s on line %d
int(-1)
Done
