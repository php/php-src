--TEST--
Test is_file() function: usage variations - links
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Do not run on Windows');
}
?>
--FILE--
<?php
/* Creating soft and hard links to a file and applying is_file() on links */

$file_path = __DIR__;
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
$file_path = __DIR__;
unlink($file_path."/is_file_variation2_symlink.tmp");
unlink($file_path."/is_file_variation2_link.tmp");
unlink($file_path."/is_file_variation2.tmp");
?>
--EXPECT--
*** Testing is_file() with links ***
bool(true)
bool(true)

*** Done ***
