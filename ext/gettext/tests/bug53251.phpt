--TEST--
Bug #53251 (bindtextdomain with null dir doesn't return old value)
--EXTENSIONS--
gettext
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) die('skip gettext leaks global state across requests');
?>
--FILE--
<?php
$results[] = is_string(bindtextdomain('foo', null));
$dir = bindtextdomain('foo', '.');
$results[] = bindtextdomain('foo', null) === $dir;
$results[] = bind_textdomain_codeset('foo', null);
$results[] = bind_textdomain_codeset('foo', 'UTF-8');
$results[] = bind_textdomain_codeset('foo', null);

$expected = [true, true, false, "UTF-8", "UTF-8"];

// musl's bindtextdomain() has no default directory to return when
// "foo" is unbound, so in the first call, you will get false instead
// of a string.
//
// bind_textdomain_codeset() always returns false on musl
// because musl only supports UTF-8. For more information:
//
//   * https://github.com/php/doc-en/issues/4311,
//   * https://github.com/php/php-src/issues/17163
//
$expected_musl = [false, true, false, false, false];

var_dump($results === $expected || $results === $expected_musl);
?>
--EXPECT--
bool(true)
