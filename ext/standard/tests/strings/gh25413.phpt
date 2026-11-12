--TEST--
GH-25413 (Frameless string predicates must preserve arguments before coercion)
--FILE--
<?php
class CoercePredicateArgument implements Stringable {
    public function __construct(private string $action) {}

    public function __toString(): string {
        switch ($this->action) {
            case 'reference needle':
                global $needle;
                $needle = $needle;
                break;
            case 'replace needle':
                $GLOBALS['needle'] = 'xyz';
                break;
            case 'unset needle':
            case 'unset referenced needle':
                unset($GLOBALS['needle']);
                break;
            case 'modify haystack':
                $GLOBALS['haystack'][0] = '!';
                break;
            case 'unset haystack':
                unset($GLOBALS['haystack']);
                break;
            case 'throw':
                throw new Exception('conversion failed');
        }
        return 'abc';
    }
}

$actions = [
    'reference needle',
    'replace needle',
    'unset needle',
    'unset referenced needle',
    'modify haystack',
    'unset haystack',
    'throw first',
    'throw second',
    'invalid first',
    'invalid second',
    'strings',
    'empty needle',
    'empty haystack',
    'different strings',
    'scalar coercion',
];

// Repeat the direct calls so the same cases can also exercise the tracing JIT.
for ($iteration = 0; $iteration < 20; $iteration++) {
    foreach (['str_contains', 'str_starts_with', 'str_ends_with'] as $function) {
        foreach ($actions as $action) {
            $results = [];
            foreach ([false, true] as $dynamic) {
                unset($haystack, $needle);
                $haystack = str_repeat('abc', random_int(1, 1));
                $needle = 'abc';
                switch ($action) {
                    case 'reference needle':
                    case 'replace needle':
                    case 'unset needle':
                        $haystack = new CoercePredicateArgument($action);
                        break;
                    case 'unset referenced needle':
                        $alias =& $needle;
                        unset($alias);
                        $haystack = new CoercePredicateArgument($action);
                        break;
                    case 'modify haystack':
                    case 'unset haystack':
                        $needle = new CoercePredicateArgument($action);
                        break;
                    case 'throw first':
                        $haystack = new CoercePredicateArgument('throw');
                        break;
                    case 'throw second':
                        $needle = new CoercePredicateArgument('throw');
                        break;
                    case 'invalid first':
                        $haystack = [];
                        break;
                    case 'invalid second':
                        $haystack = new CoercePredicateArgument('none');
                        $needle = [];
                        break;
                    case 'empty needle':
                        $needle = '';
                        break;
                    case 'empty haystack':
                        $haystack = '';
                        break;
                    case 'different strings':
                        $needle = 'xyz';
                        break;
                    case 'scalar coercion':
                        $haystack = 123;
                        $needle = 123;
                        break;
                }

                try {
                    if ($dynamic) {
                        $result = $function($haystack, $needle);
                    } else {
                        $result = match ($function) {
                            'str_contains' => str_contains($haystack, $needle),
                            'str_starts_with' => str_starts_with($haystack, $needle),
                            'str_ends_with' => str_ends_with($haystack, $needle),
                        };
                    }
                } catch (Throwable $e) {
                    $result = $e::class . ': ' . $e->getMessage();
                }
                $results[] = [$result, isset($haystack), isset($needle),
                    isset($haystack) && is_string($haystack) ? $haystack : null,
                    isset($needle) && is_string($needle) ? $needle : null];
            }
            if ($results[0] !== $results[1] && $iteration == 19) {
                echo "$function / $action: mismatch\n";
                var_dump($results);
            }
            if ($iteration == 19) {
                echo "$function / $action: ";
                var_dump($results[0][0]);
            }
        }
    }
}
?>
--EXPECT--
str_contains / reference needle: bool(true)
str_contains / replace needle: bool(true)
str_contains / unset needle: bool(true)
str_contains / unset referenced needle: bool(true)
str_contains / modify haystack: bool(true)
str_contains / unset haystack: bool(true)
str_contains / throw first: string(28) "Exception: conversion failed"
str_contains / throw second: string(28) "Exception: conversion failed"
str_contains / invalid first: string(86) "TypeError: str_contains(): Argument #1 ($haystack) must be of type string, array given"
str_contains / invalid second: string(84) "TypeError: str_contains(): Argument #2 ($needle) must be of type string, array given"
str_contains / strings: bool(true)
str_contains / empty needle: bool(true)
str_contains / empty haystack: bool(false)
str_contains / different strings: bool(false)
str_contains / scalar coercion: bool(true)
str_starts_with / reference needle: bool(true)
str_starts_with / replace needle: bool(true)
str_starts_with / unset needle: bool(true)
str_starts_with / unset referenced needle: bool(true)
str_starts_with / modify haystack: bool(true)
str_starts_with / unset haystack: bool(true)
str_starts_with / throw first: string(28) "Exception: conversion failed"
str_starts_with / throw second: string(28) "Exception: conversion failed"
str_starts_with / invalid first: string(89) "TypeError: str_starts_with(): Argument #1 ($haystack) must be of type string, array given"
str_starts_with / invalid second: string(87) "TypeError: str_starts_with(): Argument #2 ($needle) must be of type string, array given"
str_starts_with / strings: bool(true)
str_starts_with / empty needle: bool(true)
str_starts_with / empty haystack: bool(false)
str_starts_with / different strings: bool(false)
str_starts_with / scalar coercion: bool(true)
str_ends_with / reference needle: bool(true)
str_ends_with / replace needle: bool(true)
str_ends_with / unset needle: bool(true)
str_ends_with / unset referenced needle: bool(true)
str_ends_with / modify haystack: bool(true)
str_ends_with / unset haystack: bool(true)
str_ends_with / throw first: string(28) "Exception: conversion failed"
str_ends_with / throw second: string(28) "Exception: conversion failed"
str_ends_with / invalid first: string(87) "TypeError: str_ends_with(): Argument #1 ($haystack) must be of type string, array given"
str_ends_with / invalid second: string(85) "TypeError: str_ends_with(): Argument #2 ($needle) must be of type string, array given"
str_ends_with / strings: bool(true)
str_ends_with / empty needle: bool(true)
str_ends_with / empty haystack: bool(false)
str_ends_with / different strings: bool(false)
str_ends_with / scalar coercion: bool(true)
