<?php

function __autoload($classname) {
	require_once(dirname($_SERVER['PATH_TRANSLATED']).'/'.strtolower($classname).'.inc');
}

?>