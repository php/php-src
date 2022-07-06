--TEST--
Bug #69983 (get_browser fails with user agent of null)
--INI--
browscap={PWD}/browscap.ini
--ENV--
HTTP_USER_AGENT="Opera/7.11 (Windows NT 5.1; U) [en]"
--FILE--
<?php
$browser=get_browser(NULL, true);
print_r($browser);
?>
--EXPECT--
Array
(
    [browser_name_regex] => ~^.*$~
    [browser_name_pattern] => *
    [browser] => Default Browser
    [version] => 0
    [majorver] => 0
    [minorver] => 0
    [platform] => unknown
    [alpha] => 
    [beta] => 
    [win16] => 
    [win32] => 
    [win64] => 
    [frames] => 1
    [iframes] => 
    [tables] => 1
    [cookies] => 
    [backgroundsounds] => 
    [authenticodeupdate] => 0
    [cdf] => 
    [vbscript] => 
    [javaapplets] => 
    [javascript] => 
    [activexcontrols] => 
    [stripper] => 
    [isbanned] => 
    [wap] => 
    [ismobiledevice] => 
    [issyndicationreader] => 
    [crawler] => 
    [css] => 0
    [cssversion] => 0
    [supportscss] => 
    [aol] => 
    [aolversion] => 0
)
