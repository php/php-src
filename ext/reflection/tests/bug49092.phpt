--TEST--
Bug #49092 (ReflectionFunction fails to work with functions in fully qualified namespaces)
--FILE--
<?php
namespace ns;
function func(){}
new \ReflectionFunction('ns\func');
new \ReflectionFunction('\ns\func');
echo "Ok\n"
?>
--EXPECT--
Ok
