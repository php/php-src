--TEST--
Test parse_ini_file() function : variation: include path searching
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
$pwd = getcwd();
$f = basename(__FILE__);
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


var_dump(parse_ini_file($iniFile));
set_include_path($newIncludePath);
var_dump(parse_ini_file($iniFile));


$output_file = $dir2."/".$iniFile;
$iniContent = <<<FILE
error_reporting  =  E_ALL
display_errors = On
display_startup_errors = Off
log_errors = Off
log_errors_max_len = 1024
ignore_repeated_errors = Off
ignore_repeated_source = Off
report_memleaks = On
track_errors = Off
docref_root = "/phpmanual/"
docref_ext = .html

FILE;

file_put_contents($output_file, $iniContent);
var_dump(parse_ini_file($iniFile));

unlink($output_file);
foreach($newdirs as $newdir) {
   rmdir($newdir);
}




?>
===DONE===
--EXPECTF--
*** Testing parse_ini_file() : variation ***

Warning: parse_ini_file(php.ini): failed to open stream: No such file or directory in %s on line %d
bool(false)

Warning: parse_ini_file(php.ini): failed to open stream: No such file or directory in %s on line %d
bool(false)
array(11) {
  ["error_reporting"]=>
  string(4) "6143"
  ["display_errors"]=>
  string(1) "1"
  ["display_startup_errors"]=>
  string(0) ""
  ["log_errors"]=>
  string(0) ""
  ["log_errors_max_len"]=>
  string(4) "1024"
  ["ignore_repeated_errors"]=>
  string(0) ""
  ["ignore_repeated_source"]=>
  string(0) ""
  ["report_memleaks"]=>
  string(1) "1"
  ["track_errors"]=>
  string(0) ""
  ["docref_root"]=>
  string(11) "/phpmanual/"
  ["docref_ext"]=>
  string(5) ".html"
}
===DONE===
