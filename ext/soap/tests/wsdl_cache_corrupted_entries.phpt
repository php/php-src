--TEST--
soap: reject corrupted WSDL disk cache entries with invalid lengths or out-of-range table indices
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=1
soap.wsdl_cache=1
--SKIPIF--
<?php
die('skip diagnostic dichotomy');
if (PHP_INT_SIZE !== 8) {
    die('skip 64-bit only: cache format embeds 8-byte time_t and SIZEOF_ZEND_LONG=8 arch id');
}
?>
--FILE--
<?php
$dir = __DIR__ . '/ghsa_gr83_cache_dir';
@mkdir($dir);
ini_set('soap.wsdl_cache_dir', $dir);

$uri = 'https://victim.example/ghsa-gr83-j2mw-8m2h.wsdl';

/* Deterministic cache name, see get_sdl_from_cache() / put_sdl_into_cache().
 * Architecture id bytes are SIZEOF_ZEND_LONG (8) and SOAP_BIG_ENDIAN (0). */
$name = $dir . '/wsdl-' . get_current_user() . '-11' . md5($uri . chr(8) . chr(0));

$u32 = fn(int $v): string => pack('V', $v);
$u8  = fn(int $v): string => chr($v);
$str = fn(?string $s): string => $s === null ? $u32(0x7FFFFFFF) : $u32(strlen($s)) . $s;

$header = 'wsdl' . $u8(0x11) . $u8(0)
        . pack('q', time() + 86400)   /* fresh time_t, default TTL is 86400s */
        . $str($uri);

/*
 * Variant 1: sdl_deserialize_string() trusts a 32-bit length. A 1 MiB length
 * is declared with no payload at all, so a reader without end-of-buffer
 * validation would read past the emalloc(filesize) file buffer.
 */
$string_oob = $header . $u32(0x00100000);

/*
 * Variant 2: one bindings slot is allocated, then a function references
 * binding_num = 2, i.e. an index past the allocated bindings table.
 */
$index_oob = $header
    . $str('x')                 /* sdl->source    */
    . $str(null)                /* sdl->target_ns */
    . $u32(0) . $u32(0) . $u32(0) . $u32(0)  /* groups/types/elements/encoders */
    . $u32(1)                   /* num_bindings = 1 */
    . $u32(0x7FFFFFFF)          /* key: next-index */
    . $str(null)                /* binding->name     */
    . $str(null)                /* binding->location */
    . $u8(2)                    /* bindingType != BINDING_SOAP */
    . $u8(0)                    /* soap binding flag, skipped */
    . $u32(1)                   /* num_func = 1 */
    . $u32(0x7FFFFFFF)          /* function key */
    . $str(null)                /* functionName */
    . $str(null)                /* requestName  */
    . $str(null)                /* responseName */
    . $u32(2);                  /* binding_num past the single bindings slot */

foreach (['string-oob' => $string_oob, 'index-oob' => $index_oob] as $label => $payload) {
    file_put_contents($name, $payload);

    try {
        /* A corrupted cache entry must be rejected (and unlinked); the
         * constructor then fails fetching the non-existent WSDL. */
        @new SoapClient($uri, ['cache_wsdl' => WSDL_CACHE_DISK]);
    } catch (Throwable $e) {
        echo $label, " rejected\n";
    }

    var_dump(file_exists($name));
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/ghsa_gr83_cache_dir';
if (is_dir($dir)) {
    foreach (scandir($dir) as $f) {
        if ($f === '.' || $f === '..') {
            continue;
        }
        @unlink($dir . '/' . $f);
    }
    @rmdir($dir);
}
?>
--EXPECT--
string-oob rejected
bool(false)
index-oob rejected
bool(false)
