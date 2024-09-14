--TEST--
Bug #72538 (readline_redisplay crashes php)
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (!function_exists("readline_redisplay")) die("skip readline_redisplay not available");
?>
--FILE--
<?php

readline_redisplay();

?>
okey
--EXPECT--
okey
