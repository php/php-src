--TEST--
Phar: phpinfo display 4
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not loaded"); ?>
<?php if (!extension_loaded("bz2")) die("skip bz2 not loaded"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--POST--
a=b
--FILE--
<?php
ini_set('phar.extract_list', 'phar_test.phar='.dirname(__FILE__).',another.phar='. dirname(__FILE__));
phpinfo(INFO_MODULES);
ini_set('phar.readonly',1);
ini_set('phar.require_hash',1);
ini_set('phar.extract_list', '');
phpinfo(INFO_MODULES);
?>
===DONE===
--EXPECTF--
%a<br />
<h2><a name="module_Phar">Phar</a></h2>
<table border="0" cellpadding="3" width="600">
<tr class="h"><th>Phar: PHP Archive support</th><th>enabled</th></tr>
<tr><td class="e">Phar EXT version </td><td class="v">%s </td></tr>
<tr><td class="e">Phar API version </td><td class="v">1.1.1 </td></tr>
<tr><td class="e">CVS revision </td><td class="v">$Revision$ </td></tr>
<tr><td class="e">Phar-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">Tar-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">ZIP-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">gzip compression </td><td class="v">enabled </td></tr>
<tr><td class="e">bzip2 compression </td><td class="v">enabled </td></tr>
</table><br />
<table border="0" cellpadding="3" width="600">
<tr class="v"><td>
Phar based on pear/PHP_Archive, original concept by Davey Shafik.<br />Phar fully realized by Gregory Beaver and Marcus Boerger.<br />Portions of tar implementation Copyright (c) 2003-2007 Tim Kientzle.</td></tr>
</table><br />
<table border="0" cellpadding="3" width="600">
<tr class="h"><th>Directive</th><th>Local Value</th><th>Master Value</th></tr>
<tr><td class="e">phar.extract_list</td><td class="v"><ul><li>phar_test.phar => %stests</li><li>another.phar => %stests</li></ul></td><td class="v"><ul></ul></td></tr>
<tr><td class="e">phar.readonly</td><td class="v">Off</td><td class="v">Off</td></tr>
<tr><td class="e">phar.require_hash</td><td class="v">Off</td><td class="v">Off</td></tr>
</table><br />
%a<br />
<h2><a name="module_Phar">Phar</a></h2>
<table border="0" cellpadding="3" width="600">
<tr class="h"><th>Phar: PHP Archive support</th><th>enabled</th></tr>
<tr><td class="e">Phar EXT version </td><td class="v">%s </td></tr>
<tr><td class="e">Phar API version </td><td class="v">1.1.1 </td></tr>
<tr><td class="e">CVS revision </td><td class="v">$Revision$ </td></tr>
<tr><td class="e">Phar-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">Tar-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">ZIP-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">gzip compression </td><td class="v">enabled </td></tr>
<tr><td class="e">bzip2 compression </td><td class="v">enabled </td></tr>
</table><br />
<table border="0" cellpadding="3" width="600">
<tr class="v"><td>
Phar based on pear/PHP_Archive, original concept by Davey Shafik.<br />Phar fully realized by Gregory Beaver and Marcus Boerger.<br />Portions of tar implementation Copyright (c) 2003-2007 Tim Kientzle.</td></tr>
</table><br />
<table border="0" cellpadding="3" width="600">
<tr class="h"><th>Directive</th><th>Local Value</th><th>Master Value</th></tr>
<tr><td class="e">phar.extract_list</td><td class="v"><ul></ul></td><td class="v"><ul></ul></td></tr>
<tr><td class="e">phar.readonly</td><td class="v">On</td><td class="v">Off</td></tr>
<tr><td class="e">phar.require_hash</td><td class="v">On</td><td class="v">Off</td></tr>
</table><br />
%a<br />
</div></body></html>===DONE===