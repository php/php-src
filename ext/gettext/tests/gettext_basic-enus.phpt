--TEST--
Gettext basic test with en_US locale that should be on nearly every system
--EXTENSIONS--
gettext
--SKIPIF--
<?php
    
    if (!setlocale(LC_ALL, 'en_US.UTF-8')) {
        die("skip en_US.UTF-8 locale not supported.");
    }
?>
--FILE--
<?php

chdir(__DIR__);
putenv('LC_ALL=en_US.UTF-8');
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain ("messages", "./locale");
textdomain ("messages");
echo gettext("Basic test"), "\n";
echo _("Basic test"), "\n";

?>
--EXPECT--
A basic test
A basic test
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
