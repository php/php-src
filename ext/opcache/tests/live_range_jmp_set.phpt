--TEST--
Live range of ZEND_JMP_SET must not cover the fall-through path
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
/* The match always throws, so the optimizer removes the QM_ASSIGN of the else
 * branch. The JMP_SET result then shares its temporary slot with the CONCAT
 * result, which JMPZ has already freed. */
function test($a, $s) {
    try {
        if ("a" . $s) {
            $a ?: match (true) { 1 => 1 };
        }
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
test(null, "b");

function elvis($a, $b) {
    return $a ?: $b;
}
var_dump(elvis("x", "y"), elvis("", "y"), elvis(null, "z"));
echo "OK\n";
?>
--EXPECT--
UnhandledMatchError: Unhandled match case true
string(1) "x"
string(1) "y"
string(1) "z"
OK
