--TEST--
Aliasing built-in types
--FILE--
<?php

/* Taken from zend_compile.c reserved_class_names[] struct */
/* array and list are also added as they are language constructs */
$types = [
    "bool",
    "false",
    "float",
    "int",
    "null",
    "parent",
    "self",
    "static",
    "string",
    "true",
    "void",
    "never",
    "iterable",
    "object",
    "mixed",
    "array",
    "list",
];

foreach ($types as $type) {
    try {
        eval("use $type as A;");
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    try {
        eval("use $type as A; function foo$type(A \$v): A { return \$v; }");
    } catch (\Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
DONE
--EXPECT--
CompileError: Cannot alias 'bool' as it is a built-in type
CompileError: Cannot alias 'bool' as it is a built-in type
CompileError: Cannot alias 'false' as it is a built-in type
CompileError: Cannot alias 'false' as it is a built-in type
CompileError: Cannot alias 'float' as it is a built-in type
CompileError: Cannot alias 'float' as it is a built-in type
CompileError: Cannot alias 'int' as it is a built-in type
CompileError: Cannot alias 'int' as it is a built-in type
CompileError: Cannot alias 'null' as it is a built-in type
CompileError: Cannot alias 'null' as it is a built-in type
CompileError: Cannot alias 'parent' as it is a built-in type
CompileError: Cannot alias 'parent' as it is a built-in type
CompileError: Cannot alias 'self' as it is a built-in type
CompileError: Cannot alias 'self' as it is a built-in type
ParseError: syntax error, unexpected token "static"
ParseError: syntax error, unexpected token "static"
CompileError: Cannot alias 'string' as it is a built-in type
CompileError: Cannot alias 'string' as it is a built-in type
CompileError: Cannot alias 'true' as it is a built-in type
CompileError: Cannot alias 'true' as it is a built-in type
CompileError: Cannot alias 'void' as it is a built-in type
CompileError: Cannot alias 'void' as it is a built-in type
CompileError: Cannot alias 'never' as it is a built-in type
CompileError: Cannot alias 'never' as it is a built-in type
CompileError: Cannot alias 'iterable' as it is a built-in type
CompileError: Cannot alias 'iterable' as it is a built-in type
CompileError: Cannot alias 'object' as it is a built-in type
CompileError: Cannot alias 'object' as it is a built-in type
CompileError: Cannot alias 'mixed' as it is a built-in type
CompileError: Cannot alias 'mixed' as it is a built-in type
ParseError: syntax error, unexpected token "array"
ParseError: syntax error, unexpected token "array"
ParseError: syntax error, unexpected token "list"
ParseError: syntax error, unexpected token "list"
DONE
