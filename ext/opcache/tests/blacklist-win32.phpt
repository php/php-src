--TEST--
Blacklist (with glob, quote and comments)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.blacklist_filename={PWD}/opcache-*.blacklist
opcache.file_update_protection=0
opcache.file_cache_only=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (substr(PHP_OS, 0, 3) != 'WIN') {  die('skip only for Windows'); } ?>
--FILE--
<?php
$conf = opcache_get_configuration();
$conf = $conf['blacklist'];
$conf[3] = preg_replace("!^\\Q".dirname(__FILE__)."\\E!", "__DIR__", $conf[3]); 
$conf[4] = preg_replace("!^\\Q".dirname(__FILE__)."\\E!", "__DIR__", $conf[4]); 
print_r($conf);
include("blacklist.inc");
$status = opcache_get_status();
print_r(count($status['scripts']));
?>
--EXPECTF--
Array
(
    [0] => C:\path\to\foo
    [1] => C:\path\to\foo2
    [2] => C:\path\to\bar
    [3] => __DIR__\blacklist.inc
    [4] => __DIR__\current.php
    [5] => %scurrent.php
    [6] => %scurrent.php
)
ok
1
