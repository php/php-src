--TEST--
log() tests
--POST--
--GET--
--FILE--
<?php // $Id$
echo "On failure, please mail result to php-dev@lists.php.net\n";
for ($x=0, $count=0; $x < 200; $x++) {
    $x2 = (int) exp(log($x));
    // e ^ log(x) should be close in range to x
    if (($x2 < ($x + 2)) && ($x2 > ($x - 2))) { 
        $count++; 
    }
    else {
        print "$x : $x2\n";
    }
}
print $count . "\n";
--EXPECT--
On failure, please mail result to php-dev@lists.php.net
200
