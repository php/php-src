--TEST--
PFA RFC examples: "Variadics, func_get_args(), and extraneous arguments" section
--FILE--
<?php

require 'rfc_examples.inc';

function foo(int $i, int $j) {
    return func_num_args();
}

function foo2(int $i, int $j, ...$extra) {
    return func_num_args();
}

$tests = [
    'If a PFA call has no ... placeholder, then any extraneous arguments to the resulting closure will be ignored. That is consistent with how manually writing the equivalent closure would behave, and is the same regardless of whether the underlying function is variadic' => [
        foo(1, ?),
        static function (int $j) { return foo(1, $j); },
    ],
    'If a PFA call has a ... placeholder, then any extraneous arguments will be passed through to the underlying function' => [
        foo(1, ?, ...),
        static function (int $j) { return foo(1, $j, ...array_slice(func_get_args(), 1)); },
    ],
    'If a PFA call has a ... placeholder and the underlying function is variadic, then the trailing arguments will be forwarded directly but will get “collected” by the variadic parameter as normal' => [
        foo2(1, ?, ...),
        static function (int $j, ...$extra) { return foo2(1, $j, ...$extra); },
    ],
];

check_equivalence($tests);

echo "# The extra parameter here will be passed to the closure object, which will simply ignore it:\n";
var_dump(foo(1, ?)(4, 'ignore me'));

echo "# The extra parameter here will be passed to the closure object, which will forward it directly to the underlying function.  It will be accessible only via ''func_get_args()'' et al:\n";
var_dump(foo(1, ?, ...)(4, 'ignore me'));

echo "# The extra parameter here will be passed to the closure object, which will forward it directly to the underlying function.  It will show up as part of the \$extra array:\n";
var_dump(foo2(1, ?, ...)(4, 'ignore me'));

?>
==DONE==
--EXPECT--
# If a PFA call has no ... placeholder, then any extraneous arguments to the resulting closure will be ignored. That is consistent with how manually writing the equivalent closure would behave, and is the same regardless of whether the underlying function is variadic: Ok
# If a PFA call has a ... placeholder, then any extraneous arguments will be passed through to the underlying function: Ok
# If a PFA call has a ... placeholder and the underlying function is variadic, then the trailing arguments will be forwarded directly but will get “collected” by the variadic parameter as normal: Ok
# The extra parameter here will be passed to the closure object, which will simply ignore it:
int(2)
# The extra parameter here will be passed to the closure object, which will forward it directly to the underlying function.  It will be accessible only via ''func_get_args()'' et al:
int(3)
# The extra parameter here will be passed to the closure object, which will forward it directly to the underlying function.  It will show up as part of the $extra array:
int(3)
==DONE==
