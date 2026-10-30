--TEST--
Test fileinode() function: usage variations - links
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Do not run on Windows');
}
?>
--FILE--
<?php
/* Creating soft and hard links to a file and applying fileinode() on links */

$file_path = __DIR__;
fclose( fopen($file_path."/fileinode_variation1.tmp", "w") );

echo "*** Testing fileinode() with links ***\n";
/* With symlink */
symlink($file_path."/fileinode_variation1.tmp", $file_path."/fileinode_variation1_symlink.tmp");
var_dump( fileinode($file_path."/fileinode_variation1_symlink.tmp") ); //expected true
clearstatcache();

/* With hardlink */
link($file_path."/fileinode_variation1.tmp", $file_path."/fileinode_variation1_link.tmp");
var_dump( fileinode($file_path."/fileinode_variation1_link.tmp") );  // expected: true
clearstatcache();

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
unlink($file_path."/fileinode_variation1_symlink.tmp");
unlink($file_path."/fileinode_variation1_link.tmp");
unlink($file_path."/fileinode_variation1.tmp");
?>
--EXPECTF--
*** Testing fileinode() with links ***
int(%d)
int(%d)

*** Done ***
