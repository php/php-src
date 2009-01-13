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

?>
===Done===
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
array(11) {
  [u"error_reporting"]=>
  unicode(5) "32767"
  [u"display_errors"]=>
  unicode(1) "1"
  [u"display_startup_errors"]=>
  unicode(0) ""
  [u"log_errors"]=>
  unicode(0) ""
  [u"log_errors_max_len"]=>
  unicode(4) "1024"
  [u"ignore_repeated_errors"]=>
  unicode(0) ""
  [u"ignore_repeated_source"]=>
  unicode(0) ""
  [u"report_memleaks"]=>
  unicode(1) "1"
  [u"track_errors"]=>
  unicode(0) ""
  [u"docref_root"]=>
  unicode(11) "/phpmanual/"
  [u"docref_ext"]=>
  unicode(5) ".html"
}
===Done===