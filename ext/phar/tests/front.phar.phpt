--TEST--
Phar front controller with mounted external file
--INI--
default_charset=UTF-8
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/front.phar.php
REQUEST_URI=/front.phar.php/index.php
PATH_INFO=/index.php
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--FILE_EXTERNAL--
files/blog.phar
--EXPECTF--
string(%d) "<xml version="1.0" encoding="UTF-8">
<config>
 <database>
  <host>localhost</name>
  <user>squirrel</user>
  <pass>nuts</pass>
  <db>hoard</db>
 </database>
</config>
"
