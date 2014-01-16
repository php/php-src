--TEST--
Constant scalar expressions with autoloading and classes
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache') || php_sapi_name() != "cli") die("skip CLI only"); ?>
--FILE--
<?php

# This test validates that autoloading works for common const expression (AST) use cases
$classlist = [
  'A'=> 'class A { const HW = "this is A"; }',
  'B'=> 'class B extends A { const HW = parent::HW." extended by B"; }',
  'space1\C' => 'namespace space1; class C { const HW = "this is space1\C"; }',
  'D' => 'class D  { const HW = \space1\C::HW." extented by D"; }',
  'trE' => 'trait trE { public static function getHW() { return parent::HW; } }',
  'E' => 'class E extends B { use trE; }',
  'F' => 'class F { const XX = "this is F"; }',
  'G' => 'class G extends F { const XX = parent::XX." extended by G"; public static function get_me($x = "got ".self::XX) { return $x; } }',
];

spl_autoload_register(function ($class) use ($classlist) { 
	if (isset($classlist[$class])) {
		eval($classlist[$class]);
	} else {
		die("Cannot autoload $class\n");
	}
});

printf("B::HW = %s\n", B::HW);
printf("D::HW = %s\n", D::HW);

printf("E::getHW() = %s\n", E::getHW());
printf("G::get_me() = %s\n", G::get_me());

?>
--EXPECT-- 
B::HW = this is A extended by B
D::HW = this is space1\C extented by D
E::getHW() = this is A extended by B
G::get_me() = got this is F extended by G
