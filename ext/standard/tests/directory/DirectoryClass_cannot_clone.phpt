--TEST--
Cannot serialize instance of Directory class constructed via Reflection.
--FILE--
<?php

$d = dir(__DIR__);
try {
    $cloned = clone $d;
    $cloned_files = [];
    while ($row = $cloned->read()){
        $cloned_files[] = $row;
    }
    var_dump(count($cloned_files));
    echo "Using original object:\n";
    $original_files = [];
    while ($row = $d->read()){
        $original_files[] = $row;
    }
    var_dump(count($original_files));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class Directory
