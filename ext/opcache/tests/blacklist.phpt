--TEST--
Blacklist (with glob, quote and comments)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.blacklist_filename={PWD}/opcache-*.blacklist
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$conf = opcache_get_configuration();
print_r($conf['blacklist']);
?>
--EXPECT--
Array
(
    [0] => /path/to/foo
    [1] => /path/to/foo2
    [2] => /path/to/bar
)