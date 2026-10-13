--TEST--
Piping a constant into a namespaced frameless call must not release it twice
--FILE--
<?php

/* In a namespace, zend_compile_ns_call() compiles the argument list twice: once
 * for the INIT_NS_FCALL_BY_NAME fallback and once for the frameless icall. The
 * pipe operator passes its left hand side as an already compiled ZEND_AST_ZNODE,
 * so both compilations have to keep the constant alive. trim(), strtolower(),
 * dirname() and implode() all have frameless handlers. */

namespace Frameless {
    var_dump('  string literal  ' |> trim(...));
    var_dump(<<<'NOWDOC'
      nowdoc value
    NOWDOC |> trim(...));
    var_dump('MiXeD CaSe' |> strtolower(...));
    var_dump(__DIR__ |> dirname(...) === \dirname(__DIR__));
    var_dump(['a', 'b', 'c'] |> implode(...));
    var_dump('  chained call  ' |> trim(...) |> strlen(...));
}

namespace Fallback {
    function strtolower(string $string): string {
        return 'namespaced ' . \strtolower($string);
    }

    var_dump('MiXeD CaSe' |> strtolower(...));
}

?>
--EXPECT--
string(14) "string literal"
string(12) "nowdoc value"
string(10) "mixed case"
bool(true)
string(3) "abc"
int(12)
string(21) "namespaced mixed case"
