--TEST--
Test get_magic_quotes_gpc() function
--INI--
magic_quotes_gpc = 0
--FILE--
<?php
/* Prototype: int get_magic_quotes_gpc  ( void  )
 * Description: Gets the current configuration setting of magic quotes gpc
*/

echo "Simple testcase for get_magic_quotes_gpc() function\n";

$g = get_magic_quotes_gpc();
echo "\n-- magic quotes gpc set in INI file: " . $g . " --\n";

echo "\n-- Set magic quotes gpc to 1 - not allowed so should fail! --\n";
var_dump(ini_set("magic_quotes_gpc", 1));
$g = get_magic_quotes_gpc();
echo "\n-- magic quotes gpc after set: " . $g . " --\n";

echo "\n-- Set magic quotes gpc to 0:  --\n";
var_dump(ini_set("magic_quotes_gpc", 0));
$g = get_magic_quotes_gpc();
echo "\n-- magic quotes gpc after set: " . $g . " --\n";

echo "\n-- Error cases --\n"; 
// no checks on number of args
var_dump(get_magic_quotes_gpc(true)); 

?>
===DONE===
--EXPECT--
Simple testcase for get_magic_quotes_gpc() function

-- magic quotes gpc set in INI file: 0 --

-- Set magic quotes gpc to 1 - not allowed so should fail! --
bool(false)

-- magic quotes gpc after set: 0 --

-- Set magic quotes gpc to 0:  --
bool(false)

-- magic quotes gpc after set: 0 --

-- Error cases --
int(0)
===DONE===