--TEST--
JIT MUL: 005 exception and SEND optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function test($a) { 
    var_dump(+$a);
}   
    
try {
    test('foo');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}   
?>
--EXPECT--
Unsupported operand types: string * int
