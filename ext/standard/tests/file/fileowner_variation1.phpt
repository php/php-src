--TEST--
Test fileowner() function: usage variations - links
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Do not run on Windows');
}
--FILE--
<?php
/* Prototype: int fileowner ( string $filename )
 * Description: Returns the user ID of the owner of the file, or
 *              FALSE in case of an error.
 */

/* Creating soft and hard links to a file and applying fileowner() on links */

$file_path = dirname(__FILE__);
fclose( fopen($file_path."/fileowner_variation1.tmp", "w") );

echo "*** Testing fileowner() with links ***\n";
/* With symlink */
symlink($file_path."/fileowner_variation1.tmp", $file_path."/fileowner_variation1_symlink.tmp");
var_dump( fileowner($file_path."/fileowner_variation1_symlink.tmp") ); //expected true
clearstatcache();

/* With hardlink */
link($file_path."/fileowner_variation1.tmp", $file_path."/fileowner_variation1_link.tmp");
var_dump( fileowner($file_path."/fileowner_variation1_link.tmp") );  // expected: true
clearstatcache();

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
unlink($file_path."/fileowner_variation1_symlink.tmp");
unlink($file_path."/fileowner_variation1_link.tmp");
unlink($file_path."/fileowner_variation1.tmp");
?>
--EXPECTF--
*** Testing fileowner() with links ***
int(%d)
int(%d)

*** Done ***
