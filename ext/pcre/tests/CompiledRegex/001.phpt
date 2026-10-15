--TEST--
Basic CompiledRegex test
--FILE--
<?php

$o = new Regex\CompiledRegex('\D+');
var_dump($o);

?>
--EXPECT--
object(Regex\CompiledRegex)#1 (0) {
}
