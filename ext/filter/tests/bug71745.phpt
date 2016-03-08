--TEST--
Bug #71745 (FILTER_FLAG_NO_RES_RANGE does not cover whole 127.0.0.0/8 range)
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
foreach (['127.0.0.1', '127.0.0.2', '127.0.1.1', '127.127.127.127'] as $ip) {
    var_dump(filter_var($ip, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4 | FILTER_FLAG_NO_RES_RANGE));
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
