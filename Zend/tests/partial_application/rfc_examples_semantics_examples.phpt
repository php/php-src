--TEST--
PFA RFC examples: "Placeholder Semantics: Examples" section
--FILE--
<?php

require 'rfc_examples.inc';

class Point {}

function stuff(int $i1, string $s2, float $f3, Point $p4, int $m5 = 0): array {
    return [$i1, $s2, $f3, $p4, $m5];
}

function things(int $i1, ?float $f3 = null, Point ...$points): array {
    return [$i1, $f3, $points];
}

function four(int $a, int $b, int $c, int $d): string {
    return "$a, $b, $c, $d\n";
}

class E {
    private static $cache = [];

    public function __construct(private int $x, private int $y) {}

    public static function make(int $x, int $y): self {
        return self::$cache["$x-$y"] ??= new self($x, $y);
    }

    public function foo(int $a, int $b, int $c): array {
        return [$a, $b, $c];
    }
}

class RunMe {
    public function __invoke(int $a, int $b): string {
        return "$a $b";
    }
}

$point = new Point();

$tests = [
    'Manually specify the first two values, and pull the rest "as is"' => [
        stuff(?, ?, ...),
        static fn(int $i1, string $s2, float $f3, Point $p4, int $m5 = 0): array
          => stuff($i1, $s2, $f3, $p4, $m5),
    ],
    'The degenerate "first class callables" case. (Supported since 8.1)' => [
        stuff(...),
        static fn(int $i1, string $s2, float $f3, Point $p4, int $m5 = 0): array
          => stuff($i1, $s2, $f3, $p4, $m5),
    ],
    'Provide some values, require the rest to be provided later (1)' => [
        stuff(1, 'hi', ?, ?, ?),
        static fn(float $f3, Point $p4, int $m5 = 0): array => stuff(1, 'hi', $f3, $p4, $m5),
    ],
    'Provide some values, require the rest to be provided later (2)' => [
        stuff(1, 'hi', ...),
        static fn(float $f3, Point $p4, int $m5 = 0): array => stuff(1, 'hi', $f3, $p4, $m5),
    ],
    'Provide some values, but not just from the left (1)' => [
        stuff(1, ?, 3.5, ?, ?),
        static fn(string $s2, Point $p4, int $m5 = 0): array => stuff(1, $s2, 3.5, $p4, $m5),
    ],
    'Provide some values, but not just from the left (2)' => [
        stuff(1, ?, 3.5, ...),
        static fn(string $s2, Point $p4, int $m5 = 0): array => stuff(1, $s2, 3.5, $p4, $m5),
    ],
    'Provide just the last value' => [
        stuff(?, ?, ?, ?, 5),
        static fn(int $i1, string $s2, float $f3, Point $p4): array
          => stuff($i1, $s2, $f3, $p4, 5),
    ],
    'Not accounting for an optional argument means it will always get its default value' => [
        stuff(?, ?, ?, ?),
        static fn(int $i1, string $s2, float $f3, Point $p4): array
          => stuff($i1, $s2, $f3, $p4),
    ],
    'Named arguments can be pulled "out of order", and still work (1)' => [
        stuff(?, ?, f3: 3.5, p4: $point),
        static fn(int $i1, string $s2): array => stuff($i1, $s2, 3.5, $point),
    ],
    'Named arguments can be pulled "out of order", and still work (2)' => [
        stuff(?, ?, p4: $point, f3: 3.5),
        static fn(int $i1, string $s2): array => stuff($i1, $s2, 3.5, $point),
    ],

    'But named placeholders adopt the order listed' => [
        stuff(s2: ?, i1: ?, p4: ?, f3: 3.5),
        static fn(string $s2, int $i1, Point $p4): array => stuff($i1, $s2, 3.5, $p4),
    ],
    'The ... "everything else" placeholder respects named arguments' => [
        stuff(?, ?, f3: 3.5, p4: $point, ...),
        static fn(int $i1, string $s2, int $m5 = 0): array => stuff($i1, $s2, 3.5, $point, $m5),
    ],
    'Prefill all parameters, making a "delayed call" or "thunk"' => [
        stuff(1, 'hi', 3.4, $point, 5, ...),
        static fn(): array => stuff(1, 'hi', 3.4, $point, 5),
    ],

    // Variadics

    'FCC equivalent.  The signature is unchanged' => [
        things(...),
        static fn(int $i1, ?float $f3 = null, Point ...$points): array => things(...[$i1, $f3, ...$points]),
    ],
    'Provide some values, but allow the variadic to remain variadic' => [
        things(1, 3.14, ...),
        static fn(Point ...$points): array => things(1, 3.14, ...$points),
    ],
    'In this version, the partial requires precisely four arguments, the last two of which will get received by things() in the variadic parameter. Note too that $f becomes required in this case' => [
        things(?, ?, ?, ?),
        static fn(int $i1, ?float $f3, Point $points0, Point $points1): array => things($i1, $f3, $points0, $points1),
    ],

    // Esoteric examples

    'Esoteric 1' => [
        four(...),
        static fn(int $a, int $b, int $c, int $d): string => four($a, $b, $c, $d),
    ],
    'Esoteric 2' => [
        four(1, 2, ...),
        static fn(int $c, int $d): string => four(1, 2, $c, $d),
    ],
    'Esoteric 3' => [
        four(1, 2, 3, ?),
        static fn(int $d): string => four(1, 2, 3, $d),
    ],
    'Esoteric 4' => [
        four(1, ?, ?, 4),
        static fn(int $b, int $c): string => four(1, $b, $c, 4),
    ],
    'Esoteric 5' => [
        four(1, 2, 3, 4, ...),
        static fn(): string => four(1, 2, 3, 4, ...array_slice(func_get_args(), 4)),
    ],
    'Esoteric 6' => [
        four(d: 4, a: 1, ...),
        static fn(int $b, int $c): string => four(1, $b, $c, 4, ...array_slice(func_get_args(), 4)),
    ],
    'Esoteric 7' => [
        four(c: ?, d: 4, b: ?, a: 1),
        static fn(int $c, int $b): string => four(1, $b, $c, 4, ...array_slice(func_get_args(), 4)),
    ],

    // Other callable styles

    'This is allowed. Note the method is static, thus the partial closure is static' => [
        E::make(1, ?),
        static fn(int $y): E => E::make(1, $y),
    ],
    'Note the method is non-static, so the partial closure is non-static' => (function () {
        $eMaker = E::make(1, ?);
        $e = $eMaker(2);
        return [
            $e->foo(?, ?, 3),
            (function ($e) {
                return fn(int $a, int $b): array => $e->foo($a, $b, 3);
            })($e)->bindTo($e),
        ];
    })(),
    '$c can then be further refined' => (function () {
        $eMaker = E::make(1, ?);
        $e = $eMaker(2);
        $c = $e->foo(?, ?, 3);
        return [
            $c(1, ?),
            (function ($e) {
                return fn(int $b): array => $e->foo(1, $b, 3);
            })($e)->bindTo($e),
        ];
    })(),
    'RunMe' => (function () {
        $r = new RunMe();
        return [
            $r(?, 3),
            (function ($r) {
                return fn(int $a): string => $r($a, 3);
            })($r)->bindTo($r),
        ];
    })(),
];

check_equivalence($tests);

?>
--EXPECT--
# Manually specify the first two values, and pull the rest "as is": Ok
# The degenerate "first class callables" case. (Supported since 8.1): Ok
# Provide some values, require the rest to be provided later (1): Ok
# Provide some values, require the rest to be provided later (2): Ok
# Provide some values, but not just from the left (1): Ok
# Provide some values, but not just from the left (2): Ok
# Provide just the last value: Ok
# Not accounting for an optional argument means it will always get its default value: Ok
# Named arguments can be pulled "out of order", and still work (1): Ok
# Named arguments can be pulled "out of order", and still work (2): Ok
# But named placeholders adopt the order listed: Ok
# The ... "everything else" placeholder respects named arguments: Ok
# Prefill all parameters, making a "delayed call" or "thunk": Ok
# FCC equivalent.  The signature is unchanged: Ok
# Provide some values, but allow the variadic to remain variadic: Ok
# In this version, the partial requires precisely four arguments, the last two of which will get received by things() in the variadic parameter. Note too that $f becomes required in this case: Ok
# Esoteric 1: Ok
# Esoteric 2: Ok
# Esoteric 3: Ok
# Esoteric 4: Ok
# Esoteric 5: Ok
# Esoteric 6: Ok
# Esoteric 7: Ok
# This is allowed. Note the method is static, thus the partial closure is static: Ok
# Note the method is non-static, so the partial closure is non-static: Ok
# $c can then be further refined: Ok
# RunMe: Ok
