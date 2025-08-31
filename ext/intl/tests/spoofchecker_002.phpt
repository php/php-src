--TEST--
spoofchecker confusable tests
--EXTENSIONS--
intl
--SKIPIF--
<?php if(!class_exists("Spoofchecker")) print 'skip'; ?>
--FILE--
<?php

$url = "http://www.payp\xD0\xB0l.com";

$x = new Spoofchecker();
echo "Checking if words are confusable\n";
var_dump($x->areConfusable("hello, world", "goodbye, world"));
var_dump($x->areConfusable("hello, world", "hello, world"));
var_dump($x->areConfusable("hello, world", "he11o, wor1d"));
?>
--EXPECT--
Checking if words are confusable
bool(false)
bool(true)
bool(true)
