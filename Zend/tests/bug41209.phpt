--TEST--
Bug #41209 (Segmentation fault with ArrayAccess, set_error_handler and undefined var)
--FILE--
<?php

class env
{
    public function __construct()
    {
        set_error_handler(array(__CLASS__, 'errorHandler'));
    }

    public static function errorHandler($errno, $errstr, $errfile, $errline)
    {
        throw new ErrorException($errstr, 0, $errno, $errfile, $errline);
    }
}

class cache implements ArrayAccess
{
    private $container = array();

    public function offsetGet($id) {}

    public function offsetSet($id, $value) {}

    public function offsetUnset($id) {}

    public function offsetExists($id)
    {
        return isset($this->containers[(string) $id]);
    }
}

$env = new env();
$cache = new cache();
var_dump(isset($cache[$id]));

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught ErrorException: Undefined variable: id in %s:%d
Stack trace:
#0 %s(%d): env::errorHandler(8, '%s', '%s', 34, Array)
#1 {main}
  thrown in %s on line %d
