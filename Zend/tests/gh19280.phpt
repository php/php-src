--TEST--
GH-19280: Stale nInternalPosition on rehashing
--FILE--
<?php

function rehash_packed() {
    $a = range(0, 63);
    for ($i = 0; $i <= 47; $i++) {
        next($a);
    }
    for ($i = 16; $i < 62; $i++) {
        unset($a[$i]);
    }
    var_dump(key($a));
    $a[64] = 64;
    var_dump(key($a));
}

function rehash_packed_iterated() {
    $a = range(0, 63);
    for ($i = 0; $i <= 47; $i++) {
        next($a);
    }
    for ($i = 16; $i < 62; $i++) {
        unset($a[$i]);
    }
    var_dump(key($a));
    foreach ($a as &$_) {
        $a[64] = 64;
        break;
    }
    var_dump(key($a));
}

function rehash_string() {
    $a = [];
    for ($i = 0; $i < 64; $i++) {
        $a[md5($i)] = $i;
    }
    for ($i = 0; $i <= 47; $i++) {
        next($a);
    }
    for ($i = 16; $i < 62; $i++) {
        unset($a[md5($i)]);
    }
    var_dump(key($a));
    $a[md5(64)] = 64;
    var_dump(key($a));
}

function rehash_int() {
    $a = [];
    for ($i = 63; $i >= 0; $i--) {
        $a[$i] = $i;
    }
    for ($i = 0; $i <= 47; $i++) {
        next($a);
    }
    for ($i = 48; $i >= 2; $i--) {
        unset($a[$i]);
    }
    var_dump(key($a));
    $a[64] = 64;
    var_dump(key($a));
}

rehash_packed();
rehash_packed_iterated();
rehash_string();
rehash_int();

?>
--EXPECT--
int(62)
int(62)
int(62)
int(62)
string(32) "44f683a84163b3523afe57c2e008bc8c"
string(32) "44f683a84163b3523afe57c2e008bc8c"
int(1)
int(1)
