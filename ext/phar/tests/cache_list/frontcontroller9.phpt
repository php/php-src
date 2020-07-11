--TEST--
Phar front controller rewrite array [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/frontcontroller9.php
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--ENV--
SCRIPT_NAME=/frontcontroller9.php
REQUEST_URI=/frontcontroller9.php/hi
PATH_INFO=/hi
--FILE_EXTERNAL--
files/frontcontroller3.phar
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECT--
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #007700">function&nbsp;</span><span style="color: #0000BB">hio</span><span style="color: #007700">(){}</span>
</span>
</code>
