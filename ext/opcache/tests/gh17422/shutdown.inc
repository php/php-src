<?php
register_shutdown_function(static function() {
	$scripts = array_map(basename(...), array_keys(opcache_get_status()['scripts'] ?? []));
	sort($scripts);
	var_dump($scripts);
});
