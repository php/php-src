--TEST--
ob_start(): ensure multiple buffer initialization with a single call using arrays is not supported on PHP6 (http://bugs.php.net/42641)
--FILE--
<?php
/*
 * Function is implemented in main/output.c
*/

function f($string) {
    static $i=0;
    $i++;
    $len = strlen($string);
    return "f[call:$i; len:$len] - $string\n";
}

Class C {
    public $id = 'none';

    function __construct($id) {
        $this->id = $id;
    }

    static function g($string) {
        static $i=0;
        $i++;
        $len = strlen($string);
        return "C::g[call:$i; len:$len] - $string\n";
    }

    function h($string) {
        static $i=0;
        $i++;
        $len = strlen($string);
        return "C::h[call:$i; len:$len; id:$this->id] - $string\n";
    }
}

function checkAndClean() {
  var_dump(ob_list_handlers());
  while (ob_get_level()>0) {
    ob_end_flush();
  }
}

echo "\n ---> Test arrays:\n";
try {
    var_dump(ob_start(array("f")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
checkAndClean();

try {
    var_dump(ob_start(array("f", "f")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
checkAndClean();

try {
    var_dump(ob_start(array("f", "C::g", "f", "C::g")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
checkAndClean();

try {
    var_dump(ob_start(array("f", "non_existent", "f")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
checkAndClean();

try {
    var_dump(ob_start(array("f", "non_existent", "f", "f")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
checkAndClean();

$c = new c('originalID');
try {
    var_dump(ob_start(array($c, "h")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
checkAndClean();

try {
    var_dump(ob_start(array($c, "h")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
$c->id = 'changedID';
checkAndClean();

$c->id = 'changedIDagain';
try {
    var_dump(ob_start(array('f', 'C::g', array(array($c, "g"), array($c, "h")))));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
checkAndClean();
?>
--EXPECT--
---> Test arrays:
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, array callback must have exactly two members
array(0) {
}
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, class "f" not found
array(0) {
}
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, array callback must have exactly two members
array(0) {
}
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, array callback must have exactly two members
array(0) {
}
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, array callback must have exactly two members
array(0) {
}
C::h[call:1; len:51; id:originalID] - bool(true)
array(1) {
  [0]=>
  string(4) "C::h"
}

C::h[call:2; len:51; id:changedID] - bool(true)
array(1) {
  [0]=>
  string(4) "C::h"
}

TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, array callback must have exactly two members
array(0) {
}
