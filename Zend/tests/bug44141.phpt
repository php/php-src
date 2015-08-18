--TEST--
Bug #44141 (private parent constructor callable through static function)
--FILE--
<?php
class X
{
        public $x;
        private function __construct($x)
        {
                $this->x = $x;
        }
}

class Y extends X
{
        static public function cheat($x)
        {
                return new Y($x);
        }
}

$y = Y::cheat(5);
echo $y->x, PHP_EOL;
--EXPECTF--
Fatal error: Uncaught Error: Call to private X::__construct() from context 'Y' in %sbug44141.php:15
Stack trace:
#0 %s(%d): Y::cheat(5)
#1 {main}
  thrown in %sbug44141.php on line 15
