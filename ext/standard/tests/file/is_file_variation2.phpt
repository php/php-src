--TEST--
Test is_file() function: usage variations - links
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Do not run on Windows');
}
--FILE--
<?php
/* Prototype: bool is_file ( string $filename );
   Description: Tells whether the filename is a regular file
     Returns TRUE if the filename exists and is a regular file
*/

/* Creating soft and hard links to a file and applying is_file() on links */

$file_path = dirname(__FILE__);
fclose( fopen($file_path."/is_file_variation2.tmp", "w") );

echo "*** Testing is_file() with links ***\n";
/* With symlink */
symlink($file_path."/is_file_variation2.tmp", $file_path."/is_file_variation2_symlink.tmp");
var_dump( is_file($file_path."/is_file_variation2_symlink.tmp") ); //expected true
clearstatcache();

/* With hardlink */
link($file_path."/is_file_variation2.tmp", $file_path."/is_file_variation2_link.tmp");
var_dump( is_file($file_path."/is_file_variation2_link.tmp") );  // expected: true
clearstatcache();

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/is_file_variation2_symlink.tmp");
unlink($file_path."/is_file_variation2_link.tmp");
unlink($file_path."/is_file_variation2.tmp");
?>
--EXPECTF--
*** Testing is_file() with links ***
bool(true)
bool(true)

*** Done ***
