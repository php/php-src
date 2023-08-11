--TEST--
Phar front controller phps zip-based
--INI--
default_charset=UTF-8
phar.require_hash=0
--EXTENSIONS--
phar
zlib
--ENV--
SCRIPT_NAME=/frontcontroller3.phar.php
REQUEST_URI=/frontcontroller3.phar.php/a.phps
PATH_INFO=/a.phps
--FILE_EXTERNAL--
files/frontcontroller.phar.zip
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
<pre style="color: #000000"><code style="color: #0000BB">&lt;?php </code><code style="color: #007700">function </code><code style="color: #0000BB">hio</code><code style="color: #007700">(){}</code></pre>
