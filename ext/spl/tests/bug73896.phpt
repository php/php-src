--TEST--
Bug #73896 (spl_autoload() crashes when calls magic _call())
--FILE--
<?php
class Registrator {
    public static function call($callable, array  $args) {
        return call_user_func_array($callable, [$args]);
    }
}

class teLoader {
    public function __construct() {
        Registrator::call('spl_autoload_register', [$this, 'autoload']);
    }

    public function __call($method, $args) {
        $this->doSomething();
    }

    protected function autoload($class) {
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
Exception: Class 'teException' not found
