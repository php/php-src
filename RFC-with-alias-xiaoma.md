# Proposed by: [xiaoma]
# Date: 2025-5-23

# PHP RFC: Introduce `with` as an Alias for Namespace `use` Keyword

## Proposal Background

Currently in PHP, the `use` keyword serves two distinct purposes:
1. **Namespace Importing**: Used to import/alias namespaces, classes, functions, and constants
```php
use Foo\Bar as Baz;
use function Foo\hello as greet;
```
Closure Variable Capture: Used in anonymous functions to inherit variables from parent scope

Problem Analysis

Semantic Confusion: The namespace use actually performs "import/reference" operations, but its literal meaning suggests "usage", which may mislead beginners into thinking it performs file operations or immediate usage
Learning Curve: Beginners often struggle with the dual meaning of use, requiring additional context explanation
Language Consistency: Modern languages (Dart/Kotlin etc.) use more explicit terms like with or import

Proposal

Introduce with as a full alias for namespace-related use:

```php
with Foo\Bar as Baz;          // Equivalent to use Foo\Bar as Baz;
with function Foo\hello;      // Equivalent to use function Foo\hello;
```
Benefits
Clearer Semantics: with better conveys "reference/borrow" meaning, avoiding file operation misconceptions
"with this namespace..." is more accurate than "use this namespace..."

Zero-Cost Migration:

Fully backward compatible, no changes required for existing code
Developers can freely choose the more semantic keyword

Beginner Friendly:

Reduces cognitive load for newcomers, aligning with PHP's "easiest to learn" philosophy
Language Evolution: Maintains PHP's modern image while respecting historical design

Backward Compatibility

No BC breaks: Pure syntactic sugar, no behavior changes

No performance impact: Simple lexer mapping, no runtime overhead

Toolchain compatibility: All static analysis tools work without modification
Voting Suggestion

Propose as a "Language Improvement" RFC requiring 2/3 majority.

Example Comparison
```php
// Traditional syntax
use Vendor\Package\ClassName;
use function Vendor\Package\functionName;
use const Vendor\Package\CONSTANT;

// New syntax
with Vendor\Package\ClassName;
with function Vendor\Package\functionName;
with const Vendor\Package\CONSTANT;
```
FAQ
Q: Will use syntax be deprecated?

A: No, this is an optional alias mechanism. use will be permanently supported.

Q: Will closure use also get an alias?
A: Not in scope of this proposal, keeping closure syntax stable.


I know this may be an insignificant improvement, but I think it's necessary
