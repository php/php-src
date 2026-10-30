--TEST--
ZipArchive::addGlob() uses the default options when none are supplied
--EXTENSIONS--
zip
--FILE--
<?php
$dir = __DIR__ . '/addglob_default_options_dir';
@mkdir($dir);
$src = $dir . '/data.txt';
file_put_contents($src, str_repeat('The quick brown fox. ', 3000));
$archive = $dir . '/test.zip';

function stat_first(string $archive): array
{
    $zip = new ZipArchive();
    $zip->open($archive);
    $sb = $zip->statIndex(0);
    $zip->close();

    return $sb;
}

function add_glob(string $archive, string $pattern, ?array $options): bool
{
    $zip = new ZipArchive();
    $zip->open($archive, ZipArchive::CREATE | ZipArchive::OVERWRITE);
    $added = $options === null
        ? $zip->addGlob($pattern)
        : $zip->addGlob($pattern, 0, $options);
    $zip->close();

    return is_array($added);
}

/* comp_method defaults to "leave it to libzip", i.e. deflate, not CM_STORE. */
foreach (['no options' => null, 'empty options' => []] as $label => $options) {
    echo "-- $label --", PHP_EOL;
    @unlink($archive);
    var_dump(add_glob($archive, $dir . '/*.txt', $options));
    $sb = stat_first($archive);
    var_dump($sb['comp_method'] === ZipArchive::CM_DEFLATE);
    var_dump($sb['comp_size'] < $sb['size']);
}

/* flags defaults to FL_OVERWRITE, so an existing entry is replaced. */
echo '-- overwrites an existing entry --', PHP_EOL;
@unlink($archive);
$zip = new ZipArchive();
$zip->open($archive, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString($src, 'placeholder');
var_dump(is_array($zip->addGlob($dir . '/*.txt')));
$zip->close();

$zip = new ZipArchive();
$zip->open($archive);
var_dump($zip->numFiles);
var_dump($zip->getFromName($src) === file_get_contents($src));
$zip->close();
?>
--CLEAN--
<?php
$dir = __DIR__ . '/addglob_default_options_dir';
@unlink($dir . '/test.zip');
@unlink($dir . '/data.txt');
@rmdir($dir);
?>
--EXPECT--
-- no options --
bool(true)
bool(true)
bool(true)
-- empty options --
bool(true)
bool(true)
bool(true)
-- overwrites an existing entry --
bool(true)
int(1)
bool(true)
