<?php

function __autoload($file) {
	require_once(dirname($_SERVER['PATH_TRANSLATED']).'/'.strtolower($file).'.inc');
}

?>