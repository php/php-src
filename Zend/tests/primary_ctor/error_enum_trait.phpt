--TEST--
Primary constructors: not allowed on enums or traits
--FILE--
<?php
function tryCompile(string $code): void {
    try {
        eval($code);
    } catch (\ParseError $e) {
        echo $e->getMessage(), "\n";
    }
}

tryCompile('enum E(public int $x) { case A; }');
tryCompile('trait T(public int $x) {}');
?>
--EXPECT--
syntax error, unexpected token "(", expecting "{"
syntax error, unexpected token "(", expecting "{"
