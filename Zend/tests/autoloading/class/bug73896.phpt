--TEST--
Bug #73896 (autoload_register_class() crashes when calls magic __call())
--FILE--
<?php
class Registrator {
    public static function call($callable, array $args) {
        return call_user_func_array($callable, [$args]);
    }
}

class teLoader {
    public function __construct() {
        Registrator::call('autoload_register_class', [$this, 'autoload']);
    }

    public function __call($method, $args) {
        $this->doSomething();
    }

    protected function autoload($class) {
        // TODO Should this actually be called?
        die("Protected autoload() called!\n");
    }

    public function doSomething() {
        throw new teException();
    }
}

$teLoader = new teLoader();

try {
    new teChild();
} catch (Throwable $e) {
    echo "Exception: ", $e->getMessage() , "\n";
}
?>
--EXPECT--
Exception: Class "teException" not found
