--TEST--
GH-21768: Assertion failure in ReflectionProperty::is{Readable,Writable}() on internal virtual properties
--EXTENSIONS--
dom
--FILE--
<?php

$rc = new ReflectionClass('DOMDocument');
foreach ($rc->getProperties() as $rp) {
    if (!$rp->isVirtual())
	    continue;
    if (!$rp->isReadable(null))
	    die("$rp should be readable");
    if (!$rp->isWritable(null))
	    die("$rp should be writable");
}
echo "done\n";

?>
--EXPECT--
done
