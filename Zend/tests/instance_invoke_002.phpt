--TEST--
Instance invoke 
--FILE--
<?php

class Foo
{
}

(new Foo())();

--EXPECTF--
Fatal error: Function name must be a string in %sinstance_invoke_002.php on line %d
