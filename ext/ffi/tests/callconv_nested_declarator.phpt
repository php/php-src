--TEST--
FFI: calling convention inside a nested declarator
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
// The calling convention must apply to the function type whether it is
// written before the parentheses, as in "void __stdcall (*f)(void)",
// or inside them, as in "void (__stdcall *f)(void)"
$cases = [
    'declarator' => [
        'typedef void %s(%s *F)(int);',
        'F',
        fn (FFI\CType $type) => $type->getPointerType(),
    ],
    'parameter declarator' => [
        'typedef void F(void %s(%s *cb)(int));',
        'F*',
        fn (FFI\CType $type) => $type->getPointerType()->getFuncParameterType(0)->getPointerType(),
    ],
    'abstract declarator' => [
        '',
        'void %s(%s *)(int)',
        fn (FFI\CType $type) => $type->getPointerType(),
    ],
];

function func_abi(array $case, string $before, string $inside): int|string {
    [$decl, $type, $func] = $case;
    try {
        $ffi = FFI::cdef(sprintf($decl, $before, $inside));
        $type = $ffi->type(sprintf($type, $before, $inside));
        return $func($type)->getFuncABI();
    } catch (FFI\ParserException $e) {
        return $e->getMessage();
    }
}

foreach ($cases as $name => $case) {
    echo "$name:";
    foreach (['__cdecl', '__stdcall', '__fastcall', '__thiscall', '__vectorcall', '__attribute__((fastcall))'] as $callconv) {
        $expected = func_abi($case, "$callconv ", '');
        if ($expected === 'Unsupported calling convention line 1') {
            // A convention that the platform doesn't support is ignored inside the parentheses
            $expected = func_abi($case, '', '');
        }
        $actual = func_abi($case, '', "$callconv ");
        if ($expected !== $actual) {
            echo "\n  $callconv: expected ", var_export($expected, true), ", got ", var_export($actual, true);
        }
    }
    echo " done\n";
}

try {
    FFI::cdef('typedef void __cdecl (__cdecl *F)(int);');
} catch (FFI\ParserException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
declarator: done
parameter declarator: done
abstract declarator: done
Multiple calling convention specifiers at line 1
