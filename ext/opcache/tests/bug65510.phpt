--TEST--
Bug #65510 (5.5.2 crashes in _get_zval_ptr_ptr_var)
--INI--
allow_url_include=1
opcache.enable=1
opcache.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function parseQuery() {
    $m = array("l", "a", "r", "u", "e", "n", "c", "e");
    foreach($m as $n) {
        @list($a, $b) = $n;
    }
}
parseQuery();
echo "ok\n";
?>
--EXPECT--
Deprecated: Directive 'allow_url_include' is deprecated in Unknown on line 0
ok
