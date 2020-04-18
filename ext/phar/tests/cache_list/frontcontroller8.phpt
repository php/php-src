--TEST--
Phar front controller no index file 404 [cache_list]
--INI--
phar.cache_list={PWD}/frontcontroller8.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller8.php
REQUEST_URI=/frontcontroller8.php/
PATH_INFO=/
--FILE_EXTERNAL--
files/frontcontroller3.phar
--EXPECTHEADERS--
Status: 404 Not Found
--EXPECT--
<html>
 <head>
  <title>File Not Found</title>
 </head>
 <body>
  <h1>404 - File Not Found</h1>
 </body>
</html>
