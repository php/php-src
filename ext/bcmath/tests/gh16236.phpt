--TEST--
GH-16236 Segmentation fault (access null pointer) in ext/bcmath/libbcmath/src/rmzero.c:50
--EXTENSIONS--
bcmath
--FILE--
<?php
/**
 * The existing bcpow() specification returns 0 for negative powers.
 * This is mathematically incorrect and will need to be changed to raise an error at some point.
 * This test is to ensure the existing specifications until the specifications are changed.
 */
bcpow('0', '-2');

echo 'done!';
?>
--EXPECT--
done!
