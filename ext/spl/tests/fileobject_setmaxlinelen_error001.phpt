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
catch (DomainException $e) {
    echo 'DomainException thrown';
}

?>
--EXPECT--
DomainException thrown
