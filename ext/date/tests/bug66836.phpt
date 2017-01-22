--TEST--
Bug #66836 (DateTime::createFromFormat 'U' with pre 1970 dates fails parsing)
--INI--
date.timezone=Europe/Berlin
--FILE--
<?php
foreach (['-1', '-86400', '-1000000'] as $timestamp) {
    $dt = DateTime::createFromFormat('U', $timestamp);
    var_dump($dt->format('U') === $timestamp);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
