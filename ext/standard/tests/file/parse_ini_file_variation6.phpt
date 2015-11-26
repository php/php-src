--TEST--
Test parse_ini_file() function : variation - various absolute and relative paths
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : array parse_ini_file(string filename [, bool process_sections])
 * Description: Parse configuration file 
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: 
 */

echo "*** Testing parse_ini_file() : variation ***\n";
$mainDir = "parseIniFileVar6.dir";
$subDir = "parseIniFileVar6Sub";
$absMainDir = dirname(__FILE__)."/".$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir."/".$subDir;
mkdir($absSubDir);

$old_dir_path = getcwd();
chdir(dirname(__FILE__));

$allDirs = array(
  // absolute paths
  "$absSubDir/",
  "$absSubDir/../".$subDir,
  "$absSubDir//.././".$subDir,
  "$absSubDir/../../".$mainDir."/./".$subDir,
  "$absSubDir/..///".$subDir."//..//../".$subDir,
  "$absSubDir/BADDIR",
  
  // relative paths
  $mainDir."/".$subDir,
  $mainDir."//".$subDir, 
   $mainDir."///".$subDir, 
  "./".$mainDir."/../".$mainDir."/".$subDir,
  "BADDIR",  
  
);

$filename = 'ParseIniFileVar6.ini';
$content="a=test";
$absFile = $absSubDir.'/'.$filename;
$h = fopen($absFile,"w");
fwrite($h, $content);
fclose($h);

for($i = 0; $i<count($allDirs); $i++) {
  $j = $i+1;
  $dir = $allDirs[$i];
  echo "\n-- Iteration $j --\n";
  var_dump(parse_ini_file($dir."/".$filename));
}

unlink($absFile);
chdir($old_dir_path);
rmdir($absSubDir);
rmdir($absMainDir);

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing parse_ini_file() : variation ***

-- Iteration 1 --
array(1) {
  ["a"]=>
  string(4) "test"
}

-- Iteration 2 --
array(1) {
  ["a"]=>
  string(4) "test"
}

-- Iteration 3 --
array(1) {
  ["a"]=>
  string(4) "test"
}

-- Iteration 4 --
array(1) {
  ["a"]=>
  string(4) "test"
}

-- Iteration 5 --

Warning: parse_ini_file(%sparseIniFileVar6.dir/parseIniFileVar6Sub/..///parseIniFileVar6Sub//..//../parseIniFileVar6Sub/ParseIniFileVar6.ini): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Iteration 6 --

Warning: parse_ini_file(%sparseIniFileVar6.dir/parseIniFileVar6Sub/BADDIR/ParseIniFileVar6.ini): failed to open stream: No such file or directory in %s on line %d
bool(false)

-- Iteration 7 --
array(1) {
  ["a"]=>
  string(4) "test"
}

-- Iteration 8 --
array(1) {
  ["a"]=>
  string(4) "test"
}

-- Iteration 9 --
array(1) {
  ["a"]=>
  string(4) "test"
}

-- Iteration 10 --
array(1) {
  ["a"]=>
  string(4) "test"
}

-- Iteration 11 --

Warning: parse_ini_file(BADDIR/ParseIniFileVar6.ini): failed to open stream: No such file or directory in %s on line %d
bool(false)

*** Done ***