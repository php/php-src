--TEST--
Phar front controller 404 zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller6.phar.php/notfound.php
REQUEST_URI=/frontcontroller6.phar.php/notfound.php
--FILE_EXTERNAL--
frontcontroller.phar.zip
--EXPECTHEADERS--
Status: 404 Not Found
--EXPECT--
<html>
 <head>
  <title>File Not Found<title>
 </head>
 <body>
  <h1>404 - File /notfound.php Not Found</h1>
 </body>
</html>