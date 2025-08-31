--TEST--
Test is_dir() function: usage variations - diff. path notations
--FILE--
<?php
/* Passing dir names with different notations, using slashes, wild-card chars */

$file_path = __DIR__;

echo "*** Testing is_dir() with different notations of dir names ***";
$dir_name = "/is_dir_variation4";
mkdir($file_path.$dir_name);

$dirs_arr = array(
  "is_dir_variation4",
  "./is_dir_variation4",

  /* Testing a file trailing slash */
  "is_dir_variation4/",
  "./is_dir_variation4/",

  /* Testing file with double trailing slashes */
  "is_dir_variation4//",
  "./is_dir_variation4//",
  ".//is_dir_variation4//",
  "is_dir_vari*",

  /* Testing Binary safe */
  "./is_dir_variation4/".chr(0),
  "is_dir_variation4\0"
);

$count = 1;
/* loop through to test each element the above array */
foreach($dirs_arr as $dir) {
  echo "\n-- Iteration $count --\n";
  try {
    var_dump( is_dir($file_path."/".$dir ) );
  } catch (Error $e) {
    echo $e->getMessage(), "\n";
  }
  $count++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$dir_name = $file_path."/is_dir_variation4";
rmdir($dir_name);
?>
--EXPECT--
*** Testing is_dir() with different notations of dir names ***
-- Iteration 1 --
bool(true)

-- Iteration 2 --
bool(true)

-- Iteration 3 --
bool(true)

-- Iteration 4 --
bool(true)

-- Iteration 5 --
bool(true)

-- Iteration 6 --
bool(true)

-- Iteration 7 --
bool(true)

-- Iteration 8 --
bool(false)

-- Iteration 9 --
bool(false)

-- Iteration 10 --
bool(false)

*** Done ***
