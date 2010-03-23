--TEST--
Test set_magic_quotes_runtime() function -  basic test 
--INI--
magic_quotes_runtime = 0
--FILE--
<?php
/* Prototype: bool set_magic_quotes_runtime  ( int $new_setting  )
 * Description: Sets the current active configuration setting of magic_quotes_runtime
*/

echo "Simple testcase for set_magic_quotes_runtime() function - basic test\n";

$g = get_magic_quotes_runtime();
echo "\n-- magic quotes runtime set in INI file: " . $g . "--\n";

echo "\n-- Set magic quotes runtime to 1:  --\n";
var_dump(set_magic_quotes_runtime(1));
$g = get_magic_quotes_runtime();
echo "\n-- magic quotes runtime after set: " . $g . " --\n";

echo "\n-- Set magic quotes runtime to 0:  --\n";
var_dump(set_magic_quotes_runtime(0));
$g = get_magic_quotes_runtime();
echo "\n-- magic quotes runtime after set: " . $g . " --\n";

echo "\n-- Set magic quotes runtime to 1:  --\n";
var_dump(set_magic_quotes_runtime(1));
$g = get_magic_quotes_runtime();
echo "\n-- magic quotes runtime after set: " . $g . " --\n";

?>
===DONE===
--EXPECTF--
Simple testcase for set_magic_quotes_runtime() function - basic test

-- magic quotes runtime set in INI file: 0--

-- Set magic quotes runtime to 1:  --

Deprecated: Function set_magic_quotes_runtime() is deprecated in %s on line %d
bool(true)

-- magic quotes runtime after set: 1 --

-- Set magic quotes runtime to 0:  --

Deprecated: Function set_magic_quotes_runtime() is deprecated in %s on line %d
bool(true)

-- magic quotes runtime after set: 0 --

-- Set magic quotes runtime to 1:  --

Deprecated: Function set_magic_quotes_runtime() is deprecated in %s on line %d
bool(true)

-- magic quotes runtime after set: 1 --
===DONE===