--TEST--
Bug #77338: get_browser with empty string
--INI--
browscap={PWD}/browscap.ini
--FILE--
<?php

var_dump(get_browser(""));

?>
--EXPECT--
object(stdClass)#1 (34) {
  ["browser_name_regex"]=>
  string(6) "~^.*$~"
  ["browser_name_pattern"]=>
  string(1) "*"
  ["browser"]=>
  string(15) "Default Browser"
  ["version"]=>
  string(1) "0"
  ["majorver"]=>
  string(1) "0"
  ["minorver"]=>
  string(1) "0"
  ["platform"]=>
  string(7) "unknown"
  ["alpha"]=>
  string(0) ""
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(0) ""
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(1) "0"
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
  string(0) ""
  ["javascript"]=>
  string(0) ""
  ["activexcontrols"]=>
  string(0) ""
  ["stripper"]=>
  string(0) ""
  ["isbanned"]=>
  string(0) ""
  ["wap"]=>
  string(0) ""
  ["ismobiledevice"]=>
  string(0) ""
  ["issyndicationreader"]=>
  string(0) ""
  ["crawler"]=>
  string(0) ""
  ["css"]=>
  string(1) "0"
  ["cssversion"]=>
  string(1) "0"
  ["supportscss"]=>
  string(0) ""
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
