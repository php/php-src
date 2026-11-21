--TEST--
GH-25379: array_map() foreach optimization preserves callee evaluation, by-ref warnings, and dynamic call restrictions
--FILE--
<?php

namespace {
    function getf(): string {
        echo "getf\n";
        return str_repeat('strto', 1) . 'upper';
    }

    function getc(): object {
        echo "getc\n";
        return new class {
            public static function m($x) {
                return $x;
            }
        };
    }

    function getm(): string {
        echo "getm\n";
        return str_repeat('m', 1);
    }

    function throw_getm(): string {
        throw new RuntimeException("boom m");
    }

    function fn_named(int $a, int $b): int {
        return $a + $b;
    }

    function getf_named(): string {
        echo "getf_named\n";
        return str_repeat('fn_', 1) . 'named';
    }

    /* 1. Non-literal callee evaluated once and no TMP leaks on bailout */
    try {
        var_dump(array_map(getf()(...), 5));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        array_map(getf()(...), 5);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        array_map(getc()::m(...), "a" . "b");
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        array_map(getc()::{(getm())}(...), 5);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        array_map(Foo::{(getm())}(...), 5);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        array_map(getc()::{(throw_getm())}(...), 5);
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump(array_map(getf()(?, ...), [1]));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump(array_map(getf()(?, ?), [1]));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        var_dump(array_map(getf_named()(a: 1, ...), [2]));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    /* 2. By-ref callbacks emit warning and wrap instead of throwing Error */
    function inc(&$x) {
        $x++;
        return $x;
    }

    function inc2(&$x, $n) {
        $x += $n;
        return $x;
    }

    function var_ref(&...$a) {
        return $a;
    }

    class ByRef {
        public static function m(&$x) {
            $x++;
            return $x;
        }
    }

    var_dump(array_map(inc(...), [1]));
    var_dump(array_map(sort(...), [[3, 1]]));
    var_dump(array_map(inc2(?, 2), [1]));
    var_dump(array_map(var_ref(...), [1]));
    var_dump(array_map(ByRef::m(...), [1]));
    var_dump(array_map(inc2(n: 3, x: ?), [1]));

    class ByRefChild extends ByRef {
        public static function viaStatic() {
            return array_map(static::m(...), [1]);
        }
        public static function viaParent() {
            return array_map(parent::m(...), [1]);
        }
        public static function viaSelf() {
            return array_map(self::m(...), [1]);
        }
    }

    function rec(&$x) {
        static $depth = 0;
        if ($depth++) {
            return ++$x;
        }
        return array_map(rec(...), [1]);
    }

    var_dump(array_map(later(...), [1]));
    $f = 'sort';
    var_dump(array_map($f(...), [[3, 1]]));
    var_dump(ByRefChild::viaStatic());
    var_dump(ByRefChild::viaParent());
    var_dump(ByRefChild::viaSelf());
    $v = 1;
    var_dump(rec($v));
    var_dump(array_map(later2(n: 3, x: ?), [1]));

    function later(&$x) {
        return ++$x;
    }

    function later2(&$x, $n) {
        $x += $n;
        return $x;
    }

    /* 3. Dynamic-call-forbidden functions throw Error instead of reading caller scope */
    function test_forbid_dyn($a, $b) {
        try {
            array_map(extract(...), [['zz' => 1]]);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        $x = 5;
        try {
            array_map(compact(...), ['x']);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            array_map(func_get_arg(...), [0]);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            array_map(func_get_args(...), [[]]);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            array_map(func_num_args(...), [[]]);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            array_map(get_defined_vars(...), [[]]);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
    test_forbid_dyn('A', 'B');
}

namespace NsTest {
    function test_ns() {
        \var_dump(\array_map(sort(...), [[3, 1]]));
        try {
            \array_map(extract(...), [['zz' => 1]]);
        } catch (\Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
    test_ns();
}

?>
--EXPECTF--
getf
array_map(): Argument #2 ($array) must be of type array, int given
getf
array_map(): Argument #2 ($array) must be of type array, int given
getc
array_map(): Argument #2 ($array) must be of type array, string given
getc
getm
array_map(): Argument #2 ($array) must be of type array, int given
getm
Class "Foo" not found
getc
boom m
getf
array(1) {
  [0]=>
  string(1) "1"
}
getf
Partial application of strtoupper() expects at most 1 arguments, 2 given
getf_named
array(1) {
  [0]=>
  int(3)
}

Warning: inc(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(2)
}

Warning: sort(): Argument #1 ($array) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  bool(true)
}

Warning: {closure:pfa:%s}(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(3)
}

Warning: var_ref(): Argument #1 must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Warning: ByRef::m(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(2)
}

Warning: {closure:pfa:%s}(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(4)
}

Warning: later(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(2)
}

Warning: sort(): Argument #1 ($array) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  bool(true)
}

Warning: ByRef::m(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(2)
}

Warning: ByRef::m(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(2)
}

Warning: ByRef::m(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(2)
}

Warning: rec(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(2)
}

Warning: {closure:pfa:%s}(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  int(4)
}
Cannot call extract() dynamically
string(5) "unset"
Cannot call compact() dynamically
Cannot call func_get_arg() dynamically
func_get_args() expects exactly 0 arguments, 1 given
func_num_args() expects exactly 0 arguments, 1 given
get_defined_vars() expects exactly 0 arguments, 1 given

Warning: sort(): Argument #1 ($array) must be passed by reference, value given in %s on line %d
array(1) {
  [0]=>
  bool(true)
}
Cannot call extract() dynamically
