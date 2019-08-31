--TEST--
Bug #78239: Deprecation notice during string conversion converted to exception hangs
--FILE--
<?php
function handleError($level, $message, $file = '', $line = 0, $context = [])
{
    throw new ErrorException($message, 0, $level, $file, $line);
}

set_error_handler('handleError');

class A
{
    
    public function abc(): bool
    {
        return false;
    }
}

$r = new ReflectionMethod("A", "abc");
(string)$r->getReturnType() ?: "";

?>
--EXPECTF--
Fatal error: Uncaught ErrorException: Function ReflectionType::__toString() is deprecated in %s:%d
Stack trace:
#0 %s(%d): handleError(%s)
#1 {main}
  thrown in %s on line %d
