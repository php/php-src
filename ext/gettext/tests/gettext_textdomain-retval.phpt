--TEST--
Check if textdomain() returns the new domain
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
setlocale(LC_ALL, 'en_US.UTF-8');
bindtextdomain ("messages", "./locale");
echo textdomain('test'), "\n";
echo textdomain(null), "\n";
echo textdomain('foo'), "\n";

try {
	textdomain('');
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
test
test
foo
textdomain(): Argument #1 ($domain) cannot be empty
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
