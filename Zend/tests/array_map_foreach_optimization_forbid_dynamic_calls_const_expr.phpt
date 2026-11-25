--TEST--
array_map(): foreach optimization does not bypass forbid-dynamic-calls when the callable name is a compile-time constant expression
--FILE--
<?php

class A {
    const EXTRACT = 'extract';
    const EXTRACT_FQ = '\extract';
    const EXTRACT_UPPER = 'EXTRACT';
    const COMPACT = 'compact';
    const FUNC_GET_ARG = 'func_get_arg';
    const STRTOUPPER = 'strtoupper';

    public static function fcc(): void {
        try {
            array_map((self::EXTRACT)(...), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        try {
            array_map((self::EXTRACT_FQ)(...), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        try {
            array_map((self::EXTRACT_UPPER)(...), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');
    }

    public static function pfa(): void {
        try {
            array_map((self::EXTRACT)(?), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');
    }

    public static function compact(): void {
        $x = 5;
        try {
            var_dump(array_map((self::COMPACT)(...), ['x']));
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    public static function args($a, $b): void {
        try {
            var_dump(array_map((self::FUNC_GET_ARG)(...), [1, 0]));
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            var_dump(array_map((self::FUNC_GET_ARG)(?), [1, 0]));
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    public static function allowed(): void {
        var_dump(array_map((self::STRTOUPPER)(...), ['abc']));
        var_dump(array_map((self::STRTOUPPER)(?), ['abc']));
    }
}

function test_class_const(): void {
    try {
        array_map((A::EXTRACT)(...), [['zz' => 1]]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($zz ?? 'unset');
}

function test_folded_call(): void {
    try {
        array_map((chr(101) . 'xtract')(...), [['zz' => 1]]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($zz ?? 'unset');

    try {
        array_map((chr(101) . 'xtract')(?), [['zz' => 1]]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($zz ?? 'unset');

    var_dump(array_map((chr(115) . 'trtoupper')(...), ['abc']));
}

function test_direct_and_pipe(): void {
    try {
        (A::EXTRACT)(['zz' => 1]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($zz ?? 'unset');

    try {
        (chr(101) . 'xtract')(['zz' => 1]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($zz ?? 'unset');

    try {
        ['zz' => 1] |> (A::EXTRACT)(...);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($zz ?? 'unset');

    try {
        ['zz' => 1] |> (chr(101) . 'xtract')(?);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($zz ?? 'unset');

    var_dump('abc' |> (A::STRTOUPPER)(...));
}

echo "# A::fcc():\n";
A::fcc();
echo "# A::pfa():\n";
A::pfa();
echo "# A::compact():\n";
A::compact();
echo "# A::args():\n";
A::args('A', 'B');
echo "# A::allowed():\n";
A::allowed();
echo "# test_class_const():\n";
test_class_const();
echo "# test_folded_call():\n";
test_folded_call();
echo "# test_direct_and_pipe():\n";
test_direct_and_pipe();

?>
--EXPECT--
# A::fcc():
Error: Cannot call extract() dynamically
string(5) "unset"
Error: Cannot call extract() dynamically
string(5) "unset"
Error: Cannot call extract() dynamically
string(5) "unset"
# A::pfa():
Error: Cannot call extract() dynamically
string(5) "unset"
# A::compact():
Error: Cannot call compact() dynamically
# A::args():
Error: Cannot call func_get_arg() dynamically
Error: Cannot call func_get_arg() dynamically
# A::allowed():
array(1) {
  [0]=>
  string(3) "ABC"
}
array(1) {
  [0]=>
  string(3) "ABC"
}
# test_class_const():
Error: Cannot call extract() dynamically
string(5) "unset"
# test_folded_call():
Error: Cannot call extract() dynamically
string(5) "unset"
Error: Cannot call extract() dynamically
string(5) "unset"
array(1) {
  [0]=>
  string(3) "ABC"
}
# test_direct_and_pipe():
Error: Cannot call extract() dynamically
string(5) "unset"
Error: Cannot call extract() dynamically
string(5) "unset"
Error: Cannot call extract() dynamically
string(5) "unset"
Error: Cannot call extract() dynamically
string(5) "unset"
string(3) "ABC"
