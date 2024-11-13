--TEST--
Passing a too long string to ChildNode or ParentNode methods causes an exception
--EXTENSIONS--
dom
--INI--
memory_limit=-1
--SKIPIF--
<?php
if (PHP_INT_SIZE !== 8) die('skip Only for 64-bit');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
// Copied from file_get_contents_file_put_contents_5gb.phpt
function get_system_memory(): int|float|false
{
    if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
        // Windows-based memory check
        @exec('wmic OS get FreePhysicalMemory', $output);
        if (isset($output[1])) {
            return ((int)trim($output[1])) * 1024;
        }
    } else {
        // Unix/Linux-based memory check
        $memInfo = @file_get_contents("/proc/meminfo");
        if ($memInfo) {
            preg_match('/MemFree:\s+(\d+) kB/', $memInfo, $matches);
            return $matches[1] * 1024; // Convert to bytes
        }
    }
    return false;
}
if (get_system_memory() < 4 * 1024 * 1024 * 1024) {
    die('skip Reason: Insufficient RAM (less than 4GB)');
}
?>
--FILE--
<?php
$dom = new DOMDocument;
$element = $dom->appendChild($dom->createElement('root'));
$str = str_repeat('X', 2**31 + 10);
try {
    $element->append('x', $str);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $element->prepend('x', $str);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $element->after('x', $str);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $element->before('x', $str);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $element->replaceWith('x', $str);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $element->replaceChildren('x', $str);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->childNodes->count());
var_dump($element->childNodes->count());
?>
--EXPECT--
DOMElement::append(): Argument #2 must be less than or equal to 2147483647 bytes long
DOMElement::prepend(): Argument #2 must be less than or equal to 2147483647 bytes long
DOMElement::after(): Argument #2 must be less than or equal to 2147483647 bytes long
DOMElement::before(): Argument #2 must be less than or equal to 2147483647 bytes long
DOMElement::replaceWith(): Argument #2 must be less than or equal to 2147483647 bytes long
DOMElement::replaceChildren(): Argument #2 must be less than or equal to 2147483647 bytes long
int(1)
int(0)
