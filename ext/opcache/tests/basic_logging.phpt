--TEST--
Test basic logging for the Opcache
--DESCRIPTION--
This test runs a simple PHP script and ensures the Opcache 
outputs the correct logging at the highest log_verbosity_level

--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.log_verbosity_level=4
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo "Foo Bar\n";
opcache_reset();
echo "Opcache reset";
?>
--EXPECTF--
%s Message Cached script '%sbasic_logging%s'
Foo Bar
%s Debug Restart Scheduled! Reason: user
Opcache reset
