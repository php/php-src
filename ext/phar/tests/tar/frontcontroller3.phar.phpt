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
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #007700">function&nbsp;</span><span style="color: #0000BB">hio</span><span style="color: #007700">(){}</span>
</span>
</code>
