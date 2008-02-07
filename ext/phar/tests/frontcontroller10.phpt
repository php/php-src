--TEST--
Phar front controller rewrite array invalid
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller10.php
REQUEST_URI=/frontcontroller10.php/hi
PATH_INFO=/hi
--FILE_EXTERNAL--
frontcontroller4.phar
--EXPECTHEADERS--
Content-type: text/html
Status: 403 Access Denied
--EXPECT--
<html>
 <head>
  <title>Access Denied</title>
 </head>
 <body>
  <h1>403 - File /hi Access Denied</h1>
 </body>
</html>