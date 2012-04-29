<?php
$func = $argv[1];

var_dump(chdir(".."));

if ($func == 'get_current_user') {
	var_dump(get_current_user() !== "");
} else {
	var_dump($func() !== false);
}
