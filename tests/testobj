<?
define("endl","\n");

class foobar {
	function foobar() {
		$this->initialized = 1;
	}
};

class barbara extends foobar {

};

$name = "foobar";
$foo = new $name; // or die("Unable to construct foobar\n");
//print $foo->initialized;

$boo = new barbara;
print get_class($foo).endl;
print get_parent_class($foo).endl;
print get_class($boo).endl;
print get_parent_class($boo).endl;
print method_exists($foo,"foobar").endl;
print method_exists($boo,"foobar").endl;
print method_exists($boo,"barbara").endl;
//$word = new COm("word.application");
//$word->visible = true;
//sleep(5);
//$word->quit();
