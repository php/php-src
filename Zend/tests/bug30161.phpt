--TEST--
Bug #30161 (Segmentation fault with exceptions)
--FILE--
<?php
class FIIFO {

        public function __construct() {
                throw new Exception;
        }

}

class hariCow extends FIIFO {

        public function __construct() {
                try {
                        parent::__construct();
                } catch(Exception $e) {
                }
        }

        public function __toString() {
                return "ok\n";
        }

}


$db = new hariCow;

echo $db;
?>
--EXPECT--
ok
