--TEST--
Phar front controller phps
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;

$a = new Phar($fname);
$a['a.phps'] = '<?php function hio(){}';
$a->setStub('<?php
Phar::webPhar();
__HALT_COMPILER();');

$_SERVER['REQUEST_URI'] = '/' . basename(__FILE__, '.php') . '.phar.php/a.phps';
include $fname;
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phartmp.php');
__HALT_COMPILER();
?>
--EXPECTHEADERS--
Content-type: text/html
--EXPECT--
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #007700">function&nbsp;</span><span style="color: #0000BB">hio</span><span style="color: #007700">(){}</span>
</span>
</code>
