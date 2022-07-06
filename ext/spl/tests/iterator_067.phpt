--TEST--
SPL: AppendIterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myAppendIterator extends AppendIterator {}
try {
    $it = new myAppendIterator();
    echo "no exception";
} catch (InvalidArgumentException $e) {
    echo 'InvalidArgumentException thrown';
}
?>
--EXPECT--
no exception
