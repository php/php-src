--TEST--
Bug #75049 (spl_autoload_unregister can't handle spl_autoload_functions results)
--FILE--
<?php
class Auto { public static function loader() {}}
$autoloader = '\Auto::loader';

echo (int)spl_autoload_register($autoloader);
echo (int)spl_autoload_unregister($autoloader);
echo (int)spl_autoload_register($autoloader);
foreach (spl_autoload_functions() as $loader) {
	echo (int)spl_autoload_unregister($loader);
}
echo (int)count(spl_autoload_functions());
--EXPECTF--
11110
