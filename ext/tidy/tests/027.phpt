--TEST--
Bug: tidy segfaults with markup=false
--EXTENSIONS--
tidy
--FILE--
<?php

// bug report from http://sf.net/tracker/?func=detail&atid=390963&aid=1641868&group_id=27659

abstract class BaseClass {
        private static $tidyconfig;

        public function __construct() {
                self::$tidyconfig = array(
                        'indent'                        => false,
                        'clean'                         => true,
                        'merge-divs'            => false,
                        'quote-marks'           => true,
                        'drop-empty-paras'      => false,
                        'markup'                        => false,
                        'output-xhtml'          => true,
                        'wrap'                          => 0);

        }

        abstract public function run();

        public function getURL($url) {
                $data = "awerawer"; // in my code, $data is downloaded from a site

                $tidy = new tidy;
                $tidy->parseString($data, self::$tidyconfig, 'utf8');
                $tidy->cleanRepair();

                return $tidy;
        }

}

class ChildClass extends BaseClass {
        public function __construct() {
                parent::__construct();
        }

        public function run() {
                $result = $this->getURL('awer');
                if ($result === null) {
                        echo "\tError:\n";
                }
                var_dump((string)$result);
        }
}

$instance = new ChildClass();
$instance->run();

?>
--EXPECT--
string(0) ""
