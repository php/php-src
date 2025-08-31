--TEST--
Test glob() function: usage variations
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. only for Windows');
}
?>
--FILE--
<?php
echo "*** Testing glob() : usage variations ***\n";

$file_path = __DIR__;

// temp dir created
mkdir("$file_path/glob_variation");
mkdir("$file_path/glob_variation/wonder");

// temp files created
$fp = fopen("$file_path/glob_variation/wonder12345", "w");
fclose($fp);
$fp = fopen("$file_path/glob_variation/wonder;123456", "w");
fclose($fp);

$patterns = array (
  "$file_path/glob_variation/*der*",
  "$file_path/glob_variation/?onder*",
  "$file_path/glob_variation/w*der?*",
  "$file_path/glob_variation/*der5",
  "$file_path/glob_variation/??onder*",
  "$file_path/glob_variation/***der***",
  "$file_path/glob_variation/++onder*",
  "$file_path/glob_variation/WONDER5\0",
  '$file_path/glob_variation/wonder5',
  "$file_path/glob_variation/?wonder?",
  "$file_path/glob_variation/wonder?",
  TRUE  // boolean true
);
$counter = 1;
/* loop through $patterns to match each $pattern with the files created
   using glob() */
foreach($patterns as $pattern) {
  echo "\n-- Iteration $counter --\n";
  try {
    var_dump( glob($pattern) );  // default arguments
    var_dump( glob($pattern, GLOB_MARK) );
    var_dump( glob($pattern, GLOB_NOSORT) );
    var_dump( glob($pattern, GLOB_NOCHECK) );
    var_dump( glob($pattern, GLOB_NOESCAPE) );
    var_dump( glob($pattern, GLOB_ERR) );
  } catch (Error $e) {
    echo $e->getMessage(), "\n";
  }
  $counter++;
}

echo "\n*** Testing glob() with pattern within braces ***\n";
var_dump( glob("$file_path/glob_variation/*{5}", GLOB_BRACE) );

// delete temp files and dir
unlink("$file_path/glob_variation/wonder12345");
unlink("$file_path/glob_variation/wonder;123456");
rmdir("$file_path/glob_variation/wonder");
rmdir("$file_path/glob_variation");

echo "\n*** Testing glob() on directories ***\n";
// temp dir created to check for pattern matching the sub dir created in it
mkdir("$file_path/glob_variation/wonder1/wonder2", 0777, true);

$counter = 1;
/* loop through $patterns to match each $pattern with the directories created
   using glob() */
foreach($patterns as $pattern) {
  echo "-- Iteration $counter --\n";
  try {
    var_dump( glob($pattern, GLOB_ONLYDIR) );
  } catch (Error $e) {
    echo $e->getMessage(), "\n";
  }
  $counter++;
}

echo "Done\n";
?>
--CLEAN--
<?php
$file_path = __DIR__;
rmdir("$file_path/glob_variation/wonder1/wonder2");
rmdir("$file_path/glob_variation/wonder1/");
rmdir("$file_path/glob_variation/");
?>
--EXPECTF--
*** Testing glob() : usage variations ***

-- Iteration 1 --
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
  [2]=>
  string(%d) "%s/glob_variation/wonder\"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}

-- Iteration 2 --
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
  [2]=>
  string(%d) "%s/glob_variation/wonder\"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}

-- Iteration 3 --
array(2) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(2) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(2) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(2) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(2) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(2) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
}

-- Iteration 4 --
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/*der5"
}
array(0) {
}
array(0) {
}

-- Iteration 5 --
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/??onder*"
}
array(0) {
}
array(0) {
}

-- Iteration 6 --
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
  [1]=>
  string(%d) "%s/glob_variation/wonder;123456"
  [2]=>
  string(%d) "%s/glob_variation/wonder\"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}
array(3) {
  [0]=>
  string(%d) "%s/glob_variation/wonder"
  [1]=>
  string(%d) "%s/glob_variation/wonder12345"
  [2]=>
  string(%d) "%s/glob_variation/wonder;123456"
}

-- Iteration 7 --
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/++onder*"
}
array(0) {
}
array(0) {
}

-- Iteration 8 --
glob(): Argument #1 ($pattern) must not contain any null bytes

-- Iteration 9 --
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(%d) "$file_path/glob_variation/wonder5"
}
array(0) {
}
array(0) {
}

-- Iteration 10 --
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/?wonder?"
}
array(0) {
}
array(0) {
}

-- Iteration 11 --
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/wonder?"
}
array(0) {
}
array(0) {
}

-- Iteration 12 --
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(%d) "1"
}
array(0) {
}
array(0) {
}

*** Testing glob() with pattern within braces ***
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/wonder12345"
}

*** Testing glob() on directories ***
-- Iteration 1 --
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/wonder1"
}
-- Iteration 2 --
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/wonder1"
}
-- Iteration 3 --
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/wonder1"
}
-- Iteration 4 --
array(0) {
}
-- Iteration 5 --
array(0) {
}
-- Iteration 6 --
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/wonder1"
}
-- Iteration 7 --
array(0) {
}
-- Iteration 8 --
glob(): Argument #1 ($pattern) must not contain any null bytes
-- Iteration 9 --
array(0) {
}
-- Iteration 10 --
array(0) {
}
-- Iteration 11 --
array(1) {
  [0]=>
  string(%d) "%s/glob_variation/wonder1"
}
-- Iteration 12 --
array(0) {
}
Done
