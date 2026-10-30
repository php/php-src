--TEST--
PR-23720: ArrayIterator segfault in spl_array_it_get_current_data when data is NULL and by_ref is set
--CREDITS--
Georgij Tsarin crystarm@altlinux.org
Artem Burke aiburke@fobos-nt.ru
--FILE--
<?php
class Aggregator implements IteratorAggregate {
    public function getIterator(): Traversable {
        return new ArrayIterator($GLOBALS['backing']);
    }

    public function __destruct() {
        /* Runs while the temporary operand of the foreach is released by
         * ZEND_FE_RESET_RW, i.e. after the iterator has been validated but
         * before the first fetch asks for the current element. */
        unset($GLOBALS['backing']->p);
    }
}

function makeAggregator(): IteratorAggregate {
    return new Aggregator;
}

$GLOBALS['backing'] = new stdClass;
$GLOBALS['backing']->p = 1;

foreach (makeAggregator() as &$value) {
    echo "iterated\n";
}
echo "done\n";
?>
--EXPECTF--
Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
done