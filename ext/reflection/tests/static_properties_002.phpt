--TEST--
ZE2 Inheriting static properties
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class base {
	static protected $prop = 2;
	
	static function show() {
		echo __METHOD__ . '(' . self::$prop . ")\n";
	}
	
	static function inc() {
		base::$prop++;
		echo __METHOD__ . "()\n";
	}
}

class derived extends base {
	static public $prop;
	
	static function show() {
		echo __METHOD__ . '(' . self::$prop . ")\n";
	}

	static function inc() {
		derived::$prop++;
		echo __METHOD__ . "()\n";
	}
}

base::show();
derived::show();

base::inc();

base::show();
derived::show();

derived::inc();

base::show();
derived::show();

$r = new ReflectionClass('derived');
echo 'Number of properties: '. count($r->getStaticProperties()) . "\n";

echo "Done\n";
?>
--EXPECTF--
base::show(2)
derived::show(2)
base::inc()
base::show(3)
derived::show(3)
derived::inc()
base::show(4)
derived::show(4)
Number of properties: 1
Done
