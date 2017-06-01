--TEST--
int ignore_user_abort ([ bool $value ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
    die('SKIP php version so lower.');
}
?>
--FILE--
<?php
ignore_user_abort(true);
set_time_limit(0);

for($x = 0; $x<=5;$x++){
    if (connection_status() != CONNECTION_NORMAL) {
        break;
    }
}
print("Testing connection handling in PHP");
?>
--EXPECT--
Testing connection handling in PHP
