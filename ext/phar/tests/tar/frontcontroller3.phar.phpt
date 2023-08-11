--TEST--
Phar front controller phps tar-based
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller3.phar.php
REQUEST_URI=/frontcontroller3.phar.php/a.phps
PATH_INFO=/a.phps
--FILE_EXTERNAL--
files/frontcontroller.phar.tar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
<pre style="color: #000000"><code style="color: #0000BB">&lt;?php </code><code style="color: #007700">function </code><code style="color: #0000BB">hio</code><code style="color: #007700">(){}</code></pre>
