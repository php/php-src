--TEST--
GH-16390 (dba_open() can segfault for "pathless" streams)
--EXTENSIONS--
dba
--FILE--
<?php
$file = 'data:text/plain;z=y;uri=eviluri;mediatype=wut?;mediatype2=hello,somedata';
$db = dba_open($file, 'c', 'inifile');
?>
--EXPECTF--
Warning: dba_open(): Driver initialization failed for handler: inifile: Unable to determine path for locking in %s on line %d
