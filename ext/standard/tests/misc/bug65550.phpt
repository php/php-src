--TEST--
Bug #65550 (get_browser() incorrectly parses entries with "+" sign)
--INI--
browscap={PWD}/browscap.ini
--FILE--
<?php
$user_agent = 'Mozilla/5.0 (Macintosh; U; PPC Mac OS X; en-US) AppleWebKit/522+ (KHTML, like Gecko, Safari/522) OmniWeb/v613';
$caps = get_browser($user_agent, true);
var_dump($caps['browser'], $caps['version']);
?>
--EXPECT--
string(7) "OmniWeb"
string(3) "5.6"
