--TEST--
Phar: phpinfo display 4
--EXTENSIONS--
phar
zlib
bz2
--SKIPIF--
<?php
$arr = Phar::getSupportedSignatures();
if (in_array("OpenSSL", $arr)) die("skip openssl support enabled");
?>
--INI--
phar.readonly=0
phar.require_hash=0
phar.cache_list=
--POST--
a=b
--FILE--
<?php
phpinfo(INFO_MODULES);
ini_set('phar.readonly',1);
ini_set('phar.require_hash',1);
phpinfo(INFO_MODULES);
?>
===DONE===
--EXPECTF--
%a
<h2><a name="module_phar" href="#module_phar">Phar</a></h2>
<table>
<tr class="h"><th>Phar: PHP Archive support</th><th>enabled</th></tr>
<tr><td class="e">Phar API version </td><td class="v">1.1.1 </td></tr>
<tr><td class="e">Phar-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">Tar-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">ZIP-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">gzip compression </td><td class="v">enabled </td></tr>
<tr><td class="e">bzip2 compression </td><td class="v">enabled </td></tr>
<tr><td class="e">OpenSSL support </td><td class="v">disabled (install ext/openssl) </td></tr>
</table>
<table>
<tr class="v"><td>
Phar based on pear/PHP_Archive, original concept by Davey Shafik.<br />Phar fully realized by Gregory Beaver and Marcus Boerger.<br />Portions of tar implementation Copyright (c) %d-%d Tim Kientzle.</td></tr>
</table>
<table>
<tr class="h"><th>Directive</th><th>Local Value</th><th>Master Value</th></tr>
<tr><td class="e">phar.cache_list</td><td class="v"><i>no value</i></td><td class="v"><i>no value</i></td></tr>
<tr><td class="e">phar.readonly</td><td class="v">Off</td><td class="v">Off</td></tr>
<tr><td class="e">phar.require_hash</td><td class="v">Off</td><td class="v">Off</td></tr>
</table>
%a
<h2><a name="module_phar" href="#module_phar">Phar</a></h2>
<table>
<tr class="h"><th>Phar: PHP Archive support</th><th>enabled</th></tr>
<tr><td class="e">Phar API version </td><td class="v">1.1.1 </td></tr>
<tr><td class="e">Phar-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">Tar-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">ZIP-based phar archives </td><td class="v">enabled </td></tr>
<tr><td class="e">gzip compression </td><td class="v">enabled </td></tr>
<tr><td class="e">bzip2 compression </td><td class="v">enabled </td></tr>
<tr><td class="e">OpenSSL support </td><td class="v">disabled (install ext/openssl) </td></tr>
</table>
<table>
<tr class="v"><td>
Phar based on pear/PHP_Archive, original concept by Davey Shafik.<br />Phar fully realized by Gregory Beaver and Marcus Boerger.<br />Portions of tar implementation Copyright (c) %d-%d Tim Kientzle.</td></tr>
</table>
<table>
<tr class="h"><th>Directive</th><th>Local Value</th><th>Master Value</th></tr>
<tr><td class="e">phar.cache_list</td><td class="v"><i>no value</i></td><td class="v"><i>no value</i></td></tr>
<tr><td class="e">phar.readonly</td><td class="v">On</td><td class="v">Off</td></tr>
<tr><td class="e">phar.require_hash</td><td class="v">On</td><td class="v">Off</td></tr>
</table>
%a
</div></body></html>===DONE===
