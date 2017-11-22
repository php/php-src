--TEST--
Bug #39542 (Behaviour of require_once/include_once different to < 5.2.0)
--FILE--
<?php
$oldcwd = getcwd();
chdir(dirname(__FILE__));
if (substr(PHP_OS, 0, 3) == 'WIN') {
	set_include_path(dirname(__FILE__).'/bug39542;.');
} else {
	set_include_path(dirname(__FILE__).'/bug39542:.');
}

spl_autoload_register(function ($class) {
    if (!require_once($class.'.php')) {
        error_log('Error: Autoload class: '.$class.' not found!');
    }
});

new bug39542();

chdir($oldcwd);
?>
--EXPECT--
ok
