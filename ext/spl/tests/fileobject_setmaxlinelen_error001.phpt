--TEST--
SPL: SplFileObject::setMaxLineLen error 001()
--CREDITS--
Erwin Poeze <erwin.poeze at gmail.com>
--FILE--
<?php
$s = new SplFileObject( __FILE__ );
try {
    $s->setMaxLineLen(-1);
}
catch (\ValueError $e) {
	echo($e->getMessage());
}

?>
--EXPECT--
Maximum line length must be greater than or equal zero
