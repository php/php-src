--TEST--
Phar front controller phps [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller3.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller3.php
REQUEST_URI=/frontcontroller3.php/a.phps
PATH_INFO=/a.phps
--FILE_EXTERNAL--
files/frontcontroller.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #007700">function&nbsp;</span><span style="color: #0000BB">hio</span><span style="color: #007700">(){}</span>
</span>
</code>
