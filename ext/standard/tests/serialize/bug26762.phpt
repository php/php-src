--TEST--
Bug #26762 (unserialize() produces lowercase classnames)
--SKIPIF--
<?php 
	if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); 
	if (class_exists('autoload_root')) die('skip Autoload test classes exist already');
?>
--FILE--
<?php 

ini_set('unserialize_callback_func','check');

function check($name) {
	var_dump($name);
	throw new exception;
}

try {
	@unserialize('O:3:"FOO":0:{}');
}
catch (Exception $e) {
	/* ignore */
}

?>
--EXPECTF--
string(3) "FOO"
