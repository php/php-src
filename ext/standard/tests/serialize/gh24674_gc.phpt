--TEST--
GH-24674 (GC destructor running during unserialize() gets its own context)
--INI--
unserialize_callback_func=cb
--FILE--
<?php
class G {
    public $self;
    public static $hits = 0;
    public function __destruct() {
        if (self::$hits++ === 0) {
            echo "destructor: ";
            var_dump(@unserialize('R:2;'));
        }
    }
}

$keep = new stdClass;
function cb($class) { return $GLOBALS['keep']; }

for ($i = 0, $fill = 10000 - gc_status()['roots']; $i < $fill; $i++) {
    $g = new G;
    $g->self = $g;
}
unset($g);

var_dump(@unserialize('a:2:{i:0;O:8:"stdClass":0:{}i:1;O:7:"Missing":0:{}}'));
?>
--EXPECTF--
destructor: bool(false)
array(2) {
  [0]=>
  object(stdClass)#%d (0) {
  }
  [1]=>
  object(__PHP_Incomplete_Class)#%d (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(7) "Missing"
  }
}
