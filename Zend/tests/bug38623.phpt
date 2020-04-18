--TEST--
Bug #38623 (leaks in a tricky code with switch() and exceptions)
--FILE--
<?php
try {
    switch(strtolower("apache")) {
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
