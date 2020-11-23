--TEST--
Test get_browser() function : variation functionality: extra browser names
--INI--
browscap={PWD}/browscap.ini
--FILE--
<?php
$browsers = include __DIR__ . DIRECTORY_SEPARATOR . 'browsernames.inc';

echo "*** Testing get_browser() : variation functionality: extra browser names ***\n";

$count = count( $browsers );
for( $x = 20; $x < $count; $x++) {
    var_dump( get_browser( $browsers[$x], true ) );
}

?>
--EXPECT--
*** Testing get_browser() : variation functionality: extra browser names ***
array(35) {
  ["browser_name_regex"]=>
  string(94) "~^mozilla/5\.0 \(macintosh; .; .*mac os x.*\) applewebkit/.* \(.*\) version/3\.1.* safari/.*$~"
  ["browser_name_pattern"]=>
  string(78) "Mozilla/5.0 (Macintosh; ?; *Mac OS X*) AppleWebKit/* (*) Version/3.1* Safari/*"
  ["parent"]=>
  string(10) "Safari 3.1"
  ["platform"]=>
  string(6) "MacOSX"
  ["browser"]=>
  string(6) "Safari"
  ["version"]=>
  string(3) "3.1"
  ["majorver"]=>
  string(1) "3"
  ["minorver"]=>
  string(1) "1"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["backgroundsounds"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
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
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(34) {
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
array(35) {
  ["browser_name_regex"]=>
  string(67) "~^mozilla/5\.0 \(macintosh; .*mac os x.*\) gecko/.* camino/1\.5.*$~"
  ["browser_name_pattern"]=>
  string(55) "Mozilla/5.0 (Macintosh; *Mac OS X*) Gecko/* Camino/1.5*"
  ["parent"]=>
  string(6) "Camino"
  ["version"]=>
  string(3) "1.5"
  ["majorver"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "5"
  ["platform"]=>
  string(6) "MacOSX"
  ["browser"]=>
  string(6) "Camino"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(29) "~^.*konqueror/3\.1.*linux.*$~"
  ["browser_name_pattern"]=>
  string(21) "*Konqueror/3.1*Linux*"
  ["parent"]=>
  string(9) "Konqueror"
  ["version"]=>
  string(3) "3.1"
  ["majorver"]=>
  string(1) "3"
  ["minorver"]=>
  string(1) "1"
  ["browser"]=>
  string(9) "Konqueror"
  ["platform"]=>
  string(5) "Linux"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(29) "~^.*konqueror/3\.1.*linux.*$~"
  ["browser_name_pattern"]=>
  string(21) "*Konqueror/3.1*Linux*"
  ["parent"]=>
  string(9) "Konqueror"
  ["version"]=>
  string(3) "3.1"
  ["majorver"]=>
  string(1) "3"
  ["minorver"]=>
  string(1) "1"
  ["browser"]=>
  string(9) "Konqueror"
  ["platform"]=>
  string(5) "Linux"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(21) "~^.*konqueror/2\..*$~"
  ["browser_name_pattern"]=>
  string(14) "*Konqueror/2.*"
  ["parent"]=>
  string(9) "Konqueror"
  ["iframes"]=>
  string(0) ""
  ["browser"]=>
  string(9) "Konqueror"
  ["platform"]=>
  string(5) "Linux"
  ["frames"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["version"]=>
  string(1) "0"
  ["majorver"]=>
  string(1) "0"
  ["minorver"]=>
  string(1) "0"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
  string(0) ""
  ["javaapplets"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(70) "~^mozilla/5\.0 \(windows; .; windows nt 5\.1; .*rv:1\.4.*\) gecko/.*$~"
  ["browser_name_pattern"]=>
  string(58) "Mozilla/5.0 (Windows; ?; Windows NT 5.1; *rv:1.4*) Gecko/*"
  ["parent"]=>
  string(11) "Mozilla 1.4"
  ["platform"]=>
  string(5) "WinXP"
  ["win32"]=>
  string(1) "1"
  ["browser"]=>
  string(7) "Mozilla"
  ["version"]=>
  string(3) "1.4"
  ["majorver"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "4"
  ["beta"]=>
  string(1) "1"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["alpha"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(34) {
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
array(34) {
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
array(35) {
  ["browser_name_regex"]=>
  string(57) "~^mozilla/5\.0 \(x11; .*linux.*; .*rv:1\.7.*\) gecko/.*$~"
  ["browser_name_pattern"]=>
  string(44) "Mozilla/5.0 (X11; *Linux*; *rv:1.7*) Gecko/*"
  ["parent"]=>
  string(11) "Mozilla 1.7"
  ["platform"]=>
  string(5) "Linux"
  ["browser"]=>
  string(7) "Mozilla"
  ["version"]=>
  string(3) "1.7"
  ["majorver"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "7"
  ["beta"]=>
  string(1) "1"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["alpha"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(70) "~^mozilla/5\.0 \(windows; .; windows nt 5\.1; .*rv:1\.9.*\) gecko/.*$~"
  ["browser_name_pattern"]=>
  string(58) "Mozilla/5.0 (Windows; ?; Windows NT 5.1; *rv:1.9*) Gecko/*"
  ["parent"]=>
  string(11) "Mozilla 1.9"
  ["platform"]=>
  string(5) "WinXP"
  ["win32"]=>
  string(1) "1"
  ["browser"]=>
  string(7) "Mozilla"
  ["version"]=>
  string(3) "1.9"
  ["majorver"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "9"
  ["alpha"]=>
  string(1) "1"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["beta"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(34) {
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
array(35) {
  ["browser_name_regex"]=>
  string(75) "~^mozilla/5\.0 \(x11; .*; linux.*; .*; rv:1\..*\) gecko/.* firebird/0\..*$~"
  ["browser_name_pattern"]=>
  string(60) "Mozilla/5.0 (X11; *; Linux*; *; rv:1.*) Gecko/* Firebird/0.*"
  ["parent"]=>
  string(8) "Firebird"
  ["browser"]=>
  string(8) "Firebird"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(57) "~^mozilla/5\.0 \(x11; .*linux.*; .*rv:1\.4.*\) gecko/.*$~"
  ["browser_name_pattern"]=>
  string(44) "Mozilla/5.0 (X11; *Linux*; *rv:1.4*) Gecko/*"
  ["parent"]=>
  string(11) "Mozilla 1.4"
  ["platform"]=>
  string(5) "Linux"
  ["browser"]=>
  string(7) "Mozilla"
  ["version"]=>
  string(3) "1.4"
  ["majorver"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "4"
  ["beta"]=>
  string(1) "1"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["alpha"]=>
  string(0) ""
  ["win16"]=>
  string(0) ""
  ["win32"]=>
  string(0) ""
  ["win64"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(86) "~^mozilla/5\.0 \(windows; .*; windows nt 5\.1; .*; rv:1\..*\) gecko/.* firefox/0\..*$~"
  ["browser_name_pattern"]=>
  string(71) "Mozilla/5.0 (Windows; *; Windows NT 5.1; *; rv:1.*) Gecko/* Firefox/0.*"
  ["parent"]=>
  string(7) "Firefox"
  ["win32"]=>
  string(1) "1"
  ["browser"]=>
  string(7) "Firefox"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
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
  ["win64"]=>
  string(0) ""
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(41) "~^mozilla/5\.0 \(.*rv:1\.8.*\) gecko/.*$~"
  ["browser_name_pattern"]=>
  string(30) "Mozilla/5.0 (*rv:1.8*) Gecko/*"
  ["parent"]=>
  string(11) "Mozilla 1.8"
  ["browser"]=>
  string(7) "Mozilla"
  ["version"]=>
  string(3) "1.8"
  ["majorver"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "8"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(41) "~^mozilla/5\.0 \(.*rv:1\.8.*\) gecko/.*$~"
  ["browser_name_pattern"]=>
  string(30) "Mozilla/5.0 (*rv:1.8*) Gecko/*"
  ["parent"]=>
  string(11) "Mozilla 1.8"
  ["browser"]=>
  string(7) "Mozilla"
  ["version"]=>
  string(3) "1.8"
  ["majorver"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "8"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(76) "~^mozilla/5\.0 \(x11; .*; sunos.*; .*; rv:1\.8.*\) gecko/.* firefox/2\.0.*$~"
  ["browser_name_pattern"]=>
  string(61) "Mozilla/5.0 (X11; *; SunOS*; *; rv:1.8*) Gecko/* Firefox/2.0*"
  ["parent"]=>
  string(11) "Firefox 2.0"
  ["platform"]=>
  string(5) "SunOS"
  ["browser"]=>
  string(7) "Firefox"
  ["version"]=>
  string(3) "2.0"
  ["majorver"]=>
  string(1) "2"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "0"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
array(35) {
  ["browser_name_regex"]=>
  string(78) "~^mozilla/5\.0 \(x11; .*; .*linux.*; .*; rv:1\.8.*\) gecko/.* firefox/2\.0.*$~"
  ["browser_name_pattern"]=>
  string(62) "Mozilla/5.0 (X11; *; *Linux*; *; rv:1.8*) Gecko/* Firefox/2.0*"
  ["parent"]=>
  string(11) "Firefox 2.0"
  ["platform"]=>
  string(5) "Linux"
  ["browser"]=>
  string(7) "Firefox"
  ["version"]=>
  string(3) "2.0"
  ["majorver"]=>
  string(1) "2"
  ["frames"]=>
  string(1) "1"
  ["iframes"]=>
  string(1) "1"
  ["tables"]=>
  string(1) "1"
  ["cookies"]=>
  string(1) "1"
  ["javaapplets"]=>
  string(1) "1"
  ["javascript"]=>
  string(1) "1"
  ["css"]=>
  string(1) "2"
  ["cssversion"]=>
  string(1) "2"
  ["supportscss"]=>
  string(1) "1"
  ["minorver"]=>
  string(1) "0"
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
  ["backgroundsounds"]=>
  string(0) ""
  ["authenticodeupdate"]=>
  string(0) ""
  ["cdf"]=>
  string(0) ""
  ["vbscript"]=>
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
  ["aol"]=>
  string(0) ""
  ["aolversion"]=>
  string(1) "0"
}
