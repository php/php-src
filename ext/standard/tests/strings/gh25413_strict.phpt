--TEST--
GH-25413 (Frameless string predicates still reject coercion in strict mode)
--FILE--
<?php
declare(strict_types=1);

class StrictPredicateArgument implements Stringable {
    public function __toString(): string {
        throw new Exception('__toString() must not be called');
    }
}

foreach (['str_contains', 'str_starts_with', 'str_ends_with'] as $function) {
    foreach ([new StrictPredicateArgument, 123] as $invalid) {
        foreach ([1, 2] as $position) {
            $haystack = $position === 1 ? $invalid : 'abc';
            $needle = $position === 2 ? $invalid : 'abc';
            $errors = [];
            foreach ([false, true] as $dynamic) {
                try {
                    if ($dynamic) {
                        $function($haystack, $needle);
                    } else {
                        match ($function) {
                            'str_contains' => str_contains($haystack, $needle),
                            'str_starts_with' => str_starts_with($haystack, $needle),
                            'str_ends_with' => str_ends_with($haystack, $needle),
                        };
                    }
                    $errors[] = 'unexpected success';
                } catch (Throwable $e) {
                    $errors[] = $e::class . ': ' . $e->getMessage();
                }
            }
            if ($errors[0] !== $errors[1]) {
                var_dump($errors);
            }
            echo $errors[0], "\n";
        }
    }
}
?>
--EXPECT--
TypeError: str_contains(): Argument #1 ($haystack) must be of type string, StrictPredicateArgument given
TypeError: str_contains(): Argument #2 ($needle) must be of type string, StrictPredicateArgument given
TypeError: str_contains(): Argument #1 ($haystack) must be of type string, int given
TypeError: str_contains(): Argument #2 ($needle) must be of type string, int given
TypeError: str_starts_with(): Argument #1 ($haystack) must be of type string, StrictPredicateArgument given
TypeError: str_starts_with(): Argument #2 ($needle) must be of type string, StrictPredicateArgument given
TypeError: str_starts_with(): Argument #1 ($haystack) must be of type string, int given
TypeError: str_starts_with(): Argument #2 ($needle) must be of type string, int given
TypeError: str_ends_with(): Argument #1 ($haystack) must be of type string, StrictPredicateArgument given
TypeError: str_ends_with(): Argument #2 ($needle) must be of type string, StrictPredicateArgument given
TypeError: str_ends_with(): Argument #1 ($haystack) must be of type string, int given
TypeError: str_ends_with(): Argument #2 ($needle) must be of type string, int given
