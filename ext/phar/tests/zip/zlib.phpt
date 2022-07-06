--TEST--
Phar: process zlib-compressed zip alias
--INI--
phar.require_hash=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--FILE--
<?php
try {
    $a = new Phar(__DIR__ . '/files/zlib_alias.phar.zip');
    var_dump($a->getAlias());
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
string(175) "hitheresuperlongzipistoostupidtodowhatIsaysoIhavetousethisridiculouslylongaliasbecauseitisstupiddidImentionthatalreadythemadnessdoesnotstopIhateinfozipIhateinfozipIhateinfozip"
