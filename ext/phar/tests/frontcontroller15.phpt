--TEST--
Phar front controller mime type override, Phar::PHPS
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller15.php
REQUEST_URI=/frontcontroller15.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
files/frontcontroller8.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php </span><span style="color: #007700">function </span><span style="color: #0000BB">hio</span><span style="color: #007700">(){}</span></code></pre>
