--TEST--
Bug #70249 (Segmentation fault while running PHPUnit tests on phpBB 3.2-dev)
--FILE--
<?php

class Logger {
    public function __construct() {
        register_shutdown_function(function () {
            // make regular flush before other shutdown functions, which allows session data collection and so on
            $this->flush();
            // make sure log entries written by shutdown functions are also flushed
            // ensure "flush()" is called last when there are multiple shutdown functions
            register_shutdown_function([$this, 'flush'], true);
        });
    }

    public function flush($final = false) {
        return 1;
    }
}

for ($i = 0; $i < 200; $i++) {
    $a = new Logger();
}
?>
okey
--EXPECT--
okey
