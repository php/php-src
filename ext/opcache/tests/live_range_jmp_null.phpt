--TEST--
Live range of ZEND_JMP_NULL must not cover the fall-through path
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
class C {
    public $p = "p";
    function m($x) { return $x; }
}

/* The match always throws, so the optimizer removes the DO_FCALL that would
 * define the result on the non-null path. The JMP_NULL result then shares its
 * temporary slot with the CONCAT result, which JMPZ has already freed. */
function test($a, $s) {
    try {
        if ("a" . $s) {
            echo $a?->m(match (true) { 1 => 1 });
        }
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
test(new C, "b");

function nullsafe($a) {
    return [$a?->p, $a?->m("m")];
}
var_dump(nullsafe(new C), nullsafe(null));
echo "OK\n";
?>
--EXPECT--
UnhandledMatchError: Unhandled match case true
array(2) {
  [0]=>
  string(1) "p"
  [1]=>
  string(1) "m"
}
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
OK
