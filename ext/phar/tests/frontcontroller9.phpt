--TEST--
Phar front controller rewrite array
--INI--
default_charset=UTF-8
--EXTENSIONS--
phar
--ENV--
SCRIPT_NAME=/frontcontroller9.php
REQUEST_URI=/frontcontroller9.php/hi
PATH_INFO=/hi
--FILE_EXTERNAL--
files/frontcontroller3.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php </span><span style="color: #007700">function </span><span style="color: #0000BB">hio</span><span style="color: #007700">(){}</span></code></pre>
