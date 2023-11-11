--TEST--
Bug #38623 (leaks in a tricky code with switch() and exceptions)
--FILE--
<?php
/* This used to use strtolower, but opcache evaluates that at compile time as of php 8.2 https://wiki.php.net/rfc/strtolower-ascii */
function create_refcounted_string() {
    $x = 'bpache';
    $x[0] = 'a';
    return $x;
}
try {
    switch(create_refcounted_string()) {
        case "apache":
            throw new Exception("test");
            break;
    }
} catch (Exception $e) {
    echo "ok\n";
}
?>
--EXPECT--
ok
