--TEST--
Bug #26384 (domxslt->process causes segfault with xsl:key)
--SKIPIF--
<?php require_once('skipif.inc');
if (!function_exists("domxml_xslt_stylesheet_file")) { die("skip No XSLT support found");};
?>
--FILE--
<?php
$xml = domxml_open_mem(file_get_contents(dirname(__FILE__)."/area_name.xml"));
$xslt = domxml_xslt_stylesheet_file(dirname(__FILE__).'/area_list.xsl');
print $xslt->process($xml, Array(), null, null);

--EXPECT--
Object
