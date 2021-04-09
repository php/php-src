--TEST--
Bug #76711 OPcache enabled triggers false-positive "Illegal string offset"
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function test($foo) {
    var_dump(0);
    var_dump($foo[0]);
}
test("str");
?>
--EXPECT--
int(0)
string(1) "s"
