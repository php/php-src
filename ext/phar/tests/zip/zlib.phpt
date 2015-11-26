--TEST--
Phar: process zlib-compressed zip alias
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--FILE--
<?php
try {
	$a = new Phar(dirname(__FILE__) . '/files/zlib_alias.phar.zip');
	var_dump($a->getAlias());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECT--
string(175) "hitheresuperlongzipistoostupidtodowhatIsaysoIhavetousethisridiculouslylongaliasbecauseitisstupiddidImentionthatalreadythemadnessdoesnotstopIhateinfozipIhateinfozipIhateinfozip"
===DONE===
