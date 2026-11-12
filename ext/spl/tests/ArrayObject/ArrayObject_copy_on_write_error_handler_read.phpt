--TEST--
ArrayObject storage separated from the array it was built from by an error handler while reading an element
--FILE--
<?php
// The error handler writes to the object, which separates its storage from the
// array it was built from, then drops that array

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

foreach ($reads as $label => $read) {
    $source = [];
    foreach (['' => 'empty', 1 => 'one', 2 => 'two'] as $k => $v) {
        $source[$k] = $v;
    }
    $ao = new ArrayObject($source);
    set_error_handler(function ($errno, $errstr) use ($ao, &$source) {
        echo $errstr, "\n";
        $ao['new'] = 1;
        $source = null;
    });
    $result = $read($ao);
    restore_error_handler();
    echo $label, ': ', var_export($result, true), "\n";
    unset($source);
}
?>
--EXPECT--
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
