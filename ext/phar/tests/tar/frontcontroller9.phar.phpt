--TEST--
Phar front controller rewrite array tar-based
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller9.phar.php
REQUEST_URI=/frontcontroller9.phar.php/hi
PATH_INFO=/hi
--FILE_EXTERNAL--
files/frontcontroller3.phar.tar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
<pre style="color: #000000"><code style="color: #0000BB">&lt;?php </code><code style="color: #007700">function </code><code style="color: #0000BB">hio</code><code style="color: #007700">(){}</code></pre>
