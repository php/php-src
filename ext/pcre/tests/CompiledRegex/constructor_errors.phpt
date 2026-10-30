--TEST--
CompiledRegex constructor errors
--FILE--
<?php

try {
    new Regex\CompiledRegex('');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new Regex\CompiledRegex('\\C');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new Regex\CompiledRegex('(?=xyz\K)');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new Regex\CompiledRegex('\y');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Regex\CompiledRegex::__construct(): Argument #1 ($pattern) must not be empty
Regex\CompilationError: The escape sequence "\C" is not permitted
Regex\CompilationError: \K is not allowed in lookarounds (but see PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK) at offset 9
Regex\CompilationError: unrecognized character follows \ at offset 1
