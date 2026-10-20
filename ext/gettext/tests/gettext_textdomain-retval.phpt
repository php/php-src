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
	textdomain('0');
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	textdomain('');
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
test
test
foo
ValueError: textdomain(): Argument #1 ($domain) cannot be zero
ValueError: textdomain(): Argument #1 ($domain) must not be empty
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
