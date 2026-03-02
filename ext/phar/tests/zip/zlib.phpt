--TEST--
Phar: process zlib-compressed zip alias
--INI--
phar.require_hash=0
--EXTENSIONS--
phar
zlib
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
