--TEST--
Markup syntax: unqualified function components fall back to the global namespace (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
namespace {
    function greeting(string $name): \Html\Htmlable {
        return \Html\raw("Hello, {$name} (global)");
    }
}

namespace App {
    // No App\greeting exists: like an ordinary unqualified call, <Greeting/>
    // inside a namespace falls back to the global function.
    echo (<Greeting name="Liam"/>)->__toString(), "\n";
}

namespace App2 {
    function greeting(string $name): \Html\Htmlable {
        return \Html\raw("Hello, {$name} (App2)");
    }
    // A same-named function in the current namespace wins over the global one.
    echo (<Greeting name="Liam"/>)->__toString(), "\n";
}

namespace Deco {
    // Decorators receive the function actually called - here the global
    // fallback candidate, not the unresolved Deco\Greeting candidate.
    \Html\register_component_decorator(
        fn(\Html\Htmlable $h, string $c): \Html\Htmlable
            => \Html\raw((string) $h . " [decorated: {$c}]")
    );
    echo (<Greeting name="Deco"/>)->__toString(), "\n";
}
?>
--EXPECT--
Hello, Liam (global)
Hello, Liam (App2)
Hello, Deco (global) [decorated: Greeting]
