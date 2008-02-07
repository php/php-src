--TEST--
Phar front controller phps
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller3.php
REQUEST_URI=/frontcontroller3.php/a.phps
PATH_INFO=/a.phps
--FILE_EXTERNAL--
frontcontroller.phar
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #007700">function&nbsp;</span><span style="color: #0000BB">hio</span><span style="color: #007700">(){}</span>
</span>
</code>
