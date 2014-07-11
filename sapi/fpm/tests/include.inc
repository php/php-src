<?php

function get_fpm_path() /* {{{ */
{
	$php_path = getenv("TEST_PHP_EXECUTABLE");

	for ($i = 0; $i < 2; $i++) {
		$slash_pos = strrpos($php_path, "/");
		if ($slash_pos) {
			$php_path = substr($php_path, 0, $slash_pos);
		} else {
			return false;
		}
	}

	if ($php_path && is_dir($php_path) && file_exists($php_path."/fpm/php-fpm") && is_executable($php_path."/fpm/php-fpm")) { 
		/* gotcha */
		return $php_path."/fpm/php-fpm";
	}
	return false;
}
/* }}} */

function run_fpm($config, &$out = false, $extra_args = '') /* {{{ */
{
    $cfg = dirname(__FILE__).'/test-fpm-config.tmp';
    file_put_contents($cfg, $config);
    $desc = [];
    if ($out !== false) {
        $desc = [1 => array('pipe', 'w')];
    }
    /* Since it's not possible to spawn a process under linux without using a
     * shell in php (why?!?) we need a little shell trickery, so that we can
     * actually kill php-fpm */
    $fpm = proc_open('killit () { kill $child; }; trap killit TERM; '.get_fpm_path().' -F -O -y '.$cfg.' '.$extra_args.' 2>&1 & child=$!; wait', $desc, $pipes);
    register_shutdown_function(
            function($fpm) use($cfg) {
                    @unlink($cfg);
                    if (is_resource($fpm)) {
                        @proc_terminate($fpm);
                        while (proc_get_status($fpm)['running']) {
                            usleep(10000);
                        }
                    }
            },
                    $fpm
            );
    if ($out !== false) {
        $out = $pipes[1];
    }
    return $fpm;
}
/* }}} */

function run_fpm_till($needle, $config, $max = 10) /* {{{ */
{
    $i = 0;
    $fpm = run_fpm($config, $tail);
    if (is_resource($fpm)) {
        while($i < $max) {
            $i++;
            $line = fgets($tail);
            if(preg_match($needle, $line) === 1) {
                break;
            }
        }
        if ($i >= $max) {
            $line = false;
        }
        proc_terminate($fpm);
        stream_get_contents($tail);
        fclose($tail);
        proc_close($fpm);
    }
    return $line;
}
/* }}} */

?>
