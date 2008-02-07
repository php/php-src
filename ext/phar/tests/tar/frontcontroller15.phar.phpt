--TEST--
Phar front controller mime type override, Phar::PHPS tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller15.phar.php
REQUEST_URI=/frontcontroller15.phar.php/a.php
PATH_INFO=/a.php
--FILE_EXTERNAL--
frontcontroller8.phar.tar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #007700">function&nbsp;</span><span style="color: #0000BB">hio</span><span style="color: #007700">(){}</span>
</span>
</code>

