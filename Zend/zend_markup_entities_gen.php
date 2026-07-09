<?php
/**
 * Generates zend_markup_entities.h — the named character reference table used
 * by native markup text/attribute decoding (RFC: Native Markup Expressions).
 *
 * Usage: php Zend/zend_markup_entities_gen.php > Zend/zend_markup_entities.h
 *
 * The source data is ext/standard/html_tables/ents_html5.txt — the same
 * WHATWG HTML5 named-reference table that feeds html_entity_decode(), so
 * there is a single copy of the data in the tree. The HTML standard
 * guarantees this list is frozen — no named entity will ever be added — so
 * the generated header only changes if this generator does. The table
 * contains only the canonical semicolon-terminated forms: markup requires
 * the well-formed "&name;" spelling and treats legacy forms like "&amp"
 * as literal text.
 */

$source = $argv[1] ?? __DIR__ . '/../ext/standard/html_tables/ents_html5.txt';

$lines = @file($source, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
if ($lines === false) {
    fwrite(STDERR, "cannot read {$source}\n");
    exit(1);
}

function utf8_bytes(int $cp): string {
    if ($cp < 0x80) {
        return chr($cp);
    } elseif ($cp < 0x800) {
        return chr(0xC0 | ($cp >> 6)) . chr(0x80 | ($cp & 0x3F));
    } elseif ($cp < 0x10000) {
        return chr(0xE0 | ($cp >> 12)) . chr(0x80 | (($cp >> 6) & 0x3F)) . chr(0x80 | ($cp & 0x3F));
    }
    return chr(0xF0 | ($cp >> 18)) . chr(0x80 | (($cp >> 12) & 0x3F))
        . chr(0x80 | (($cp >> 6) & 0x3F)) . chr(0x80 | ($cp & 0x3F));
}

$entities = [];
foreach ($lines as $line) {
    $parts = explode(' ', trim($line));
    $name = array_shift($parts);
    if (!preg_match('/^[A-Za-z][A-Za-z0-9]*$/', $name)) {
        fwrite(STDERR, "unexpected entity name: $name\n");
        exit(1);
    }
    if ($parts === []) {
        fwrite(STDERR, "no codepoints for entity: $name\n");
        exit(1);
    }
    $utf8 = '';
    foreach ($parts as $hex) {
        if (!preg_match('/^[0-9A-F]{1,6}$/', $hex)) {
            fwrite(STDERR, "unexpected codepoint \"$hex\" for entity: $name\n");
            exit(1);
        }
        $utf8 .= utf8_bytes(hexdec($hex));
    }
    $entities[$name] = $utf8;
}

/* Byte order, matching the C-side memcmp binary search. */
ksort($entities, SORT_STRING);

$maxName = max(array_map('strlen', array_keys($entities)));
$maxUtf8 = max(array_map('strlen', $entities));
$count = count($entities);

echo <<<EOT
/* This is a generated file — edit Zend/zend_markup_entities_gen.php instead.
 * Source data: ext/standard/html_tables/ents_html5.txt (the WHATWG HTML5
 * named-reference table, frozen by the HTML standard; also feeds
 * html_entity_decode()). Canonical semicolon-terminated forms only; sorted
 * by name in byte order for bsearch. */

#ifndef ZEND_MARKUP_ENTITIES_H
#define ZEND_MARKUP_ENTITIES_H

typedef struct _zend_markup_entity {
	const char *name;   /* without the leading '&' and trailing ';' */
	const char *utf8;   /* replacement text, UTF-8 */
	uint8_t name_len;
	uint8_t utf8_len;
} zend_markup_entity;

#define ZEND_MARKUP_ENTITY_COUNT {$count}
#define ZEND_MARKUP_ENTITY_MAX_NAME {$maxName}
#define ZEND_MARKUP_ENTITY_MAX_UTF8 {$maxUtf8}

static const zend_markup_entity zend_markup_entities[ZEND_MARKUP_ENTITY_COUNT] = {

EOT;

foreach ($entities as $name => $utf8) {
    $hex = '';
    foreach (str_split($utf8) as $byte) {
        $hex .= sprintf('\\x%02X', ord($byte));
    }
    printf("\t{\"%s\", \"%s\", %d, %d},\n", $name, $hex, strlen($name), strlen($utf8));
}

echo <<<EOT
};

#endif /* ZEND_MARKUP_ENTITIES_H */

EOT;
