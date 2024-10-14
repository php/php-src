--TEST--
Live range & free on return
--FILE--
<?php
class bar {
        public $foo = 1;
        public $bar = 1;

        function __destruct() {
                throw $this->foo;
        }
}
foreach (new bar as &$foo) {
        try {
                $foo = new Exception;
                return; // frees the loop variable
        } catch (Exception $e) {
                echo "exception\n";
        }
}
echo "end\n";
?>
--EXPECTF--
Fatal error: Uncaught Exception in %stemporary_cleaning_009.php:12
Stack trace:
#0 {main}
  thrown in %stemporary_cleaning_009.php on line 12
