<?php
class Loader {
	static private $loader;

	static function getLoader() {
		if (null !== self::$loader) {
			return self::$loader;
		}
		return self::$loader = new Loader();
	}		

	static function getCounter() {
		static $counter = 0;
		return $counter++;
	}
}

Loader::getLoader();
Loader::getCounter();
Loader::getCounter();
