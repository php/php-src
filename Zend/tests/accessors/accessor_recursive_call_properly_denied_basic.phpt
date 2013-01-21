--TEST--
ZE2 Tests that recursive accessor calls are denied and emit warning upon detection
--FILE--
<?php

class base {
	public $var {
		get {
			echo '__METHOD__ = '.__METHOD__.PHP_EOL.PHP_EOL;
			interloper::call_get($this);
			return $this->var; 
		}
		set {
			echo '__METHOD__ = '.__METHOD__.PHP_EOL.PHP_EOL;
			interloper::call_set($this, $value);
			$this->var = $value;
		}
		isset {
			echo '__METHOD__ = '.__METHOD__.PHP_EOL.PHP_EOL;
			interloper::call_isset($this);
			return $this->var !== NULL;
		}
		unset {
			echo '__METHOD__ = '.__METHOD__.PHP_EOL.PHP_EOL;
			interloper::call_unset($this);
			$this->var = NULL;
		}
	}
}

class interloper {
	public static function call_set($obj, $value) {
		echo "interloper::call_set('$value');".PHP_EOL;
		$obj->var = $value;
	}
	public static function call_get($obj) {
		echo "interloper::call_get();".PHP_EOL;
		echo "\$obj->var = ".var_export($obj->var, true).PHP_EOL;
	}
	public static function call_isset($obj) {
		echo "interloper::call_isset();".PHP_EOL;
		isset($obj->var);
	}
	public static function call_unset($obj) {
		echo "interloper::call_unset();".PHP_EOL;
		unset($obj->var);
	}
}

function Title($s) {
	echo PHP_EOL.PHP_EOL.$s.PHP_EOL;
	echo str_repeat('-', strlen($s)).PHP_EOL;
}


$o = new base();

Title('Setting value');
echo '$o->var = 5;'.PHP_EOL;
$o->var = 5;
var_dump($o);

Title('Getting value');
echo 'echo $o->var;'.PHP_EOL;
echo $o->var.PHP_EOL;

Title('Checking value via isset()');
echo 'isset($o->var);'.PHP_EOL;
var_dump(isset($o->var));

Title('unset()ing value');
echo 'unset($o->var);'.PHP_EOL;
unset($o->var);

Title('Final results of object');
var_dump($o);

?>
===DONE===
--EXPECTF--
Setting value
-------------
$o->var = 5;
__METHOD__ = base::$var->set

interloper::call_set('5');

Warning: Recursive access to base::$var->set detected, call ignored. in %s on line %d
object(base)#1 (1) {
  ["var"]=>
  int(5)
}


Getting value
-------------
echo $o->var;
__METHOD__ = base::$var->get

interloper::call_get();

Warning: Recursive access to base::$var->get detected, return value is NULL. in %s on line %d
$obj->var = NULL
5


Checking value via isset()
--------------------------
isset($o->var);
__METHOD__ = base::$var->isset

interloper::call_isset();

Warning: Recursive access to base::$var->isset detected, call ignored. in %s on line %d
__METHOD__ = base::$var->get

interloper::call_get();

Warning: Recursive access to base::$var->get detected, return value is NULL. in %s on line %d
$obj->var = NULL
bool(true)


unset()ing value
----------------
unset($o->var);
__METHOD__ = base::$var->unset

interloper::call_unset();

Warning: Recursive access to base::$var->unset detected, call ignored. in %s on line %d
__METHOD__ = base::$var->set

interloper::call_set('');

Warning: Recursive access to base::$var->set detected, call ignored. in %s on line %d


Final results of object
-----------------------
object(base)#1 (1) {
  ["var"]=>
  NULL
}
===DONE===