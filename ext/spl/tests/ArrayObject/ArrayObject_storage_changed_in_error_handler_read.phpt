--TEST--
ArrayObject storage separated or replaced by an error handler while reading an element
--FILE--
<?php
// Null and fractional offsets are deprecated: the error handler runs while the
// element is being looked up

$handlers = [
    'replaced' => function ($ao) {
        $ao->exchangeArray(['' => 'other empty', 1 => 'other one']);
    },
    'separated' => function ($ao) {
        $data = $ao->__serialize();
        $ao['new'] = 1;
        // Frees the table that the storage was separated from
        unset($data);
    },
];
$reads = [
    '$ao[null]' => fn ($ao) => $ao[null],
    'isset($ao[null])' => fn ($ao) => isset($ao[null]),
    'empty($ao[null])' => fn ($ao) => empty($ao[null]),
    '$ao->offsetGet(null)' => fn ($ao) => $ao->offsetGet(null),
    '$ao->offsetExists(null)' => fn ($ao) => $ao->offsetExists(null),
    '$ao[1.5]' => fn ($ao) => $ao[1.5],
    'isset($ao[1.5])' => fn ($ao) => isset($ao[1.5]),
    'seek(1.5)' => function ($ao) {
        $it = $ao->getIterator();
        $it->seek(1.5);
        return $it->current();
    },
];

foreach ($handlers as $how => $handler) {
    echo "-- $how\n";
    foreach ($reads as $label => $read) {
        $ao = new ArrayObject(['' => 'empty', 1 => 'one', 2 => 'two']);
        set_error_handler(function ($errno, $errstr) use ($ao, $handler) {
            echo $errstr, "\n";
            $handler($ao);
        });
        $result = $read($ao);
        restore_error_handler();
        echo $label, ': ', var_export($result, true), "\n";
    }
}
?>
--EXPECT--
-- replaced
Using null as an array offset is deprecated, use an empty string instead
$ao[null]: 'other empty'
Using null as an array offset is deprecated, use an empty string instead
isset($ao[null]): true
Using null as an array offset is deprecated, use an empty string instead
empty($ao[null]): false
Using null as an array offset is deprecated, use an empty string instead
$ao->offsetGet(null): 'other empty'
Using null as an array offset is deprecated, use an empty string instead
$ao->offsetExists(null): true
Implicit conversion from float 1.5 to int loses precision
$ao[1.5]: 'other one'
Implicit conversion from float 1.5 to int loses precision
isset($ao[1.5]): true
Implicit conversion from float 1.5 to int loses precision
seek(1.5): 'other one'
-- separated
Using null as an array offset is deprecated, use an empty string instead
$ao[null]: 'empty'
Using null as an array offset is deprecated, use an empty string instead
isset($ao[null]): true
Using null as an array offset is deprecated, use an empty string instead
empty($ao[null]): false
Using null as an array offset is deprecated, use an empty string instead
$ao->offsetGet(null): 'empty'
Using null as an array offset is deprecated, use an empty string instead
$ao->offsetExists(null): true
Implicit conversion from float 1.5 to int loses precision
$ao[1.5]: 'one'
Implicit conversion from float 1.5 to int loses precision
isset($ao[1.5]): true
Implicit conversion from float 1.5 to int loses precision
seek(1.5): 'one'
