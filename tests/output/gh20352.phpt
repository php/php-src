--TEST--
GH-20352 (UAF in php_output_handler_free via re-entrant ob_start() during error deactivation)
--FILE--
<?php
class Test {
    public function __destruct() {
        // Spray output stack
        for ($i = 0; $i < 1000; $i++)
            ob_start(static function() {});
    }

    public function __invoke($x) {
        // Trigger php_output_deactivate() through forbidden operation
        ob_start('foo');
        return $x;
    }
}

ob_start(new Test, 1);

echo "trigger bug";
?>
--EXPECTF--
Fatal error: ob_start(): Cannot use output buffering in output buffering display handlers in %s on line %d
