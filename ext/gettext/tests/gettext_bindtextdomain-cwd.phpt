--TEST--
Test if bindtextdomain() returns string id if no directory path is set(if directory path is 'null')
--EXTENSIONS--
gettext
--SKIPIF--
<?php

if (!setlocale(LC_ALL, 'en_US.UTF-8')) {
    die("skip en_US.UTF-8 locale not supported.");
}
if (getenv('SKIP_REPEAT')) {
    die('skip gettext leaks global state across requests');
}
?>
--FILE--
<?php
$base_dir = __DIR__;
chdir($base_dir);
putenv('LC_ALL=en_US.UTF-8');
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain('messages',null);
var_dump(gettext('Basic test'));
bindtextdomain('messages', './locale');
var_dump(gettext('Basic test'));

?>
--EXPECT--
string(10) "Basic test"
string(12) "A basic test"
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-09
