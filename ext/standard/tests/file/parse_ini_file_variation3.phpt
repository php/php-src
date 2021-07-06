--TEST--
Test parse_ini_file() function : variation: include path searching
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing parse_ini_file() : variation ***\n";
$pwd = getcwd();
$f = "parse_ini_file_variation3";
$dir1 = $pwd."/".$f.".dir1";
$dir2 = $pwd."/".$f.".dir2";
$dir3 = $pwd."/".$f.".dir3";
$iniFile = "php.ini";

$newdirs = array($dir1, $dir2, $dir3);
$pathSep = ":";
$newIncludePath = "";
if(substr(PHP_OS, 0, 3) == 'WIN' ) {
   $pathSep = ";";
}
foreach($newdirs as $newdir) {
   mkdir($newdir);
   $newIncludePath .= $newdir.$pathSep;
}

set_include_path($newIncludePath);
$path = get_include_path();
echo "New include path is : " . $path . "\n";

$output_file = $dir2."/".$iniFile;
$iniContent = <<<FILE
error_reporting  =  E_ALL
display_errors = On
display_startup_errors = Off
log_errors = Off
ignore_repeated_errors = Off
ignore_repeated_source = Off
report_memleaks = On
docref_root = "/phpmanual/"
docref_ext = .html

FILE;

file_put_contents($output_file, $iniContent);
var_dump(parse_ini_file($iniFile));

?>
--CLEAN--
<?php

$pwd = getcwd();
$f = "parse_ini_file_variation3";
$iniFile = "php.ini";

$dir1 = $pwd."/".$f.".dir1";
$dir2 = $pwd."/".$f.".dir2";
$dir3 = $pwd."/".$f.".dir3";
$newdirs = array($dir1, $dir2, $dir3);
$output_file = $dir2."/".$iniFile;

// Tidy up after test
unlink($output_file);
foreach($newdirs as $newdir) {
   rmdir($newdir);
}

?>
--EXPECTF--
*** Testing parse_ini_file() : variation ***
New include path is : %sparse_ini_file_variation3.dir1%sparse_ini_file_variation3.dir2%sparse_ini_file_variation3.dir3%S
array(9) {
  ["error_reporting"]=>
  string(5) "32767"
  ["display_errors"]=>
  string(1) "1"
  ["display_startup_errors"]=>
  string(0) ""
  ["log_errors"]=>
  string(0) ""
  ["ignore_repeated_errors"]=>
  string(0) ""
  ["ignore_repeated_source"]=>
  string(0) ""
  ["report_memleaks"]=>
  string(1) "1"
  ["docref_root"]=>
  string(11) "/phpmanual/"
  ["docref_ext"]=>
  string(5) ".html"
}
