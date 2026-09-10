--TEST--
Block pass must not strip a JMP across an unreachable block that is kept for its loop var frees
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
--ENV--
USE_ZEND_ALLOC=0
USE_TRACKED_ALLOC=1
--FILE--
<?php
function test($a, $b, $c) {
    do {
        if ($a) {
            switch ($b[0]) {
                case 'x':
                    switch ($c[0]) {
                        default:
                            return "returned";
                    }
                default:
                    continue 2;
            }
        }
    } while (false);
    return $b[0];
}

// Take the "continue 2" path with a refcounted switch subject.
var_dump(test(true, [uniqid("p") !== "" ? "y" . uniqid("") : ""], ["z"]) !== "");
// Take the "case 'x'" path.
var_dump(test(true, ["x"], ["z"]));
// Take the path that skips the switch entirely.
var_dump(test(false, ["y"], ["z"]));
echo "OK\n";
?>
--EXPECT--
bool(true)
string(8) "returned"
string(1) "y"
OK
