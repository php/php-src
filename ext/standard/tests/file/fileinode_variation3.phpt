--TEST--
Test fileinode() function: usage variations - diff. path notations
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Passing file names with different notations, using slashes, wild-card chars */

$file_path = __DIR__;

echo "*** Testing fileinode() with different notations of file names ***\n";
$dir_name = $file_path."/fileinode_variation3";
mkdir($dir_name);
$file_handle = fopen($dir_name."/fileinode_variation3.tmp", "w");
fclose($file_handle);

$files_arr = array(
  "/fileinode_variation3/fileinode_variation3.tmp",

  /* Testing a file trailing slash */
  "/fileinode_variation3/fileinode_variation3.tmp/",

  /* Testing file with double slashes */
  "/fileinode_variation3//fileinode_variation3.tmp",
  "//fileinode_variation3//fileinode_variation3.tmp",
  "/fileinode_variation3/*.tmp",
  "fileinode_variation3/fileinode*.tmp",

  /* Testing Binary safe */
  "/fileinode_variation3/fileinode_variation3.tmp".chr(0),
  "/fileinode_variation3/fileinode_variation3.tmp\0"
);

$count = 1;
/* loop through to test each element in the above array */
foreach($files_arr as $file) {
  echo "- Iteration $count -\n";
  try {
    var_dump( fileinode( $file_path."/".$file ) );
  } catch (Error $e) {
    echo $e->getMessage(), "\n";
  }
  clearstatcache();
  $count++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$dir_name = $file_path."/fileinode_variation3";
unlink($dir_name."/fileinode_variation3.tmp");
rmdir($dir_name);
?>
--EXPECTF--
*** Testing fileinode() with different notations of file names ***
- Iteration 1 -
int(%i)
- Iteration 2 -

Warning: fileinode(): stat failed for %s//fileinode_variation3/fileinode_variation3.tmp/ in %s on line %d
bool(false)
- Iteration 3 -
int(%i)
- Iteration 4 -
int(%i)
- Iteration 5 -

Warning: fileinode(): stat failed for %s//fileinode_variation3/*.tmp in %s on line %d
bool(false)
- Iteration 6 -

Warning: fileinode(): stat failed for %s/fileinode_variation3/fileinode*.tmp in %s on line %d
bool(false)
- Iteration 7 -

Warning: fileinode(): Filename contains null byte in %s on line %d
bool(false)
- Iteration 8 -

Warning: fileinode(): Filename contains null byte in %s on line %d
bool(false)

*** Done ***
