--TEST--
OSS-Fuzz #416302790
--FILE--
<?php
function x(){
    #[Attr] const X = 1;
}
?>
--EXPECTF--
Parse error: syntax error, unexpected token "const" in %s on line %d
