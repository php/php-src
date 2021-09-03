--TEST--
Phar front controller 404 tar-based
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller6.phar.php
REQUEST_URI=/frontcontroller6.phar.php/notfound.php
PATH_INFO=/notfound.php
--FILE_EXTERNAL--
files/frontcontroller.phar.tar
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
