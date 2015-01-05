--TEST--
Test if bindtextdomain() returns string id if no directory path is set(if directory path is 'null')
--SKIPIF--
<?php
if (!extension_loaded("gettext")) {
    die("skip gettext extension is not loaded.\n");
}
if (!setlocale(LC_ALL, 'en_US.UTF-8')) {
    die("skip en_US.UTF-8 locale not supported.");
}
--FILE--
<?php
$base_dir = dirname(__FILE__);
chdir($base_dir);
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain('messages',null);
var_dump(gettext('Basic test'));
bindtextdomain('messages', './locale');
var_dump(gettext('Basic test'));

?>
--EXPECTF--
string(10) "Basic test"
string(12) "A basic test"
--CREDIT--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-09
