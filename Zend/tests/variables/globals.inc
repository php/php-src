<?php

var_dump(isset($_SERVER));
var_dump(empty($_SERVER));
var_dump(gettype($_SERVER));
var_dump(count($_SERVER));

var_dump($_SERVER['PHP_SELF']);
unset($_SERVER['PHP_SELF']);
var_dump($_SERVER['PHP_SELF']);

unset($_SERVER);
var_dump($_SERVER);

?>
