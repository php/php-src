--TEST--
fileinfo CDF: NUL-terminate directory names and stop on zero-length catalog records
--EXTENSIONS--
fileinfo
--SKIPIF--
<?php
die('skip diagnostic dichotomy');
?>
--FILE--
<?php
/*
 * Constructs a minimal OLE2/CFB compound file (512-byte sectors, 64-byte
 * mini sectors) hosting a SummaryInformation stream (one CDF_NULL property, so
 * cdf_check_summary_info() reaches the directory-name loop in MIME mode) and a
 * Catalog stream whose records end with a zero-length record.
 *
 * Two variants, both exercised through finfo_buffer(FILEINFO_MIME_TYPE):
 *  - the fourth directory entry fills all 32 UTF-16 name units with nonzero
 *    characters, so the directory-name copy must reserve room for a
 *    terminator and cannot rely on the entry itself being NUL-terminated;
 *  - all directory names are terminated, while the Catalog stream still
 *    contains the zero-length record that the catalog parser must handle
 *    without re-processing the same record.
 */
function build_cdf(bool $unterminatedName): string
{
    $FREE = 0xFFFFFFFF;
    $ENDOFCHAIN = 0xFFFFFFFE;
    $FATSECT = 0xFFFFFFFD;
    $sector = 512;
    $mini = 64;

    $u16 = fn(int $v): string => pack('v', $v);
    $u32 = fn(int $v): string => pack('V', $v);
    $utf16le = static function (string $s): string {
        $out = '';
        foreach (str_split($s) as $c) {
            $out .= pack('v', ord($c));
        }
        return $out . "\x00\x00";
    };

    $dirEntry = function (string $name, int $type, int $firstSector,
        int $size, bool $root = false) use ($u16, $u32, $utf16le): string {
        $raw = $utf16le($name);
        $e = str_repeat("\0", 128);
        $e = substr_replace($e, $raw, 0, strlen($raw));
        $e = substr_replace($e, $u16(strlen($raw)), 64, 2);
        $e[66] = chr($type);
        $e[67] = chr(1);
        $e = substr_replace($e, pack('i', -1), 68, 4);
        $e = substr_replace($e, pack('i', -1), 72, 4);
        $e = substr_replace($e, pack('i', $root ? 1 : -1), 76, 4);
        $e = substr_replace($e, $u32($firstSector), 116, 4);
        $e = substr_replace($e, $u32($size), 120, 4);
        return $e;
    };

    // SummaryInformation: valid summary header, one CDF_NULL property.
    $summary = str_repeat("\0", 128);
    $summary = substr_replace($summary, $u16(0xFFFE), 0, 2);
    $summary = substr_replace($summary, $u16(0x0602), 4, 2);
    $summary = substr_replace($summary, $u16(2), 6, 2);
    $summary = substr_replace($summary, $u32(1), 24, 4);          // si_count
    $summary = substr_replace($summary, $u32(0x30), 0x1c + 16, 4); // sd_offset
    $summary = substr_replace($summary, $u32(0x18), 0x30, 4);      // sh_len
    $summary = substr_replace($summary, $u32(1), 0x34, 4);        // sh_properties
    $summary = substr_replace($summary, $u32(1), 0x38, 4);        // pi_id
    $summary = substr_replace($summary, $u32(0x10), 0x3c, 4);     // offset
    $summary = substr_replace($summary, $u32(1), 0x40, 4);        // CDF_NULL
    $summary = substr($summary, 0, 68);

    // Catalog: two 2-byte records followed by a zero-length record.
    $catalog = "\x02\x00\x02\x00\x00\x00";

    $miniStream = str_repeat("\0", $sector);
    $miniStream = substr_replace($miniStream, $summary, 0, strlen($summary));
    $miniStream = substr_replace($miniStream, $catalog, 2 * $mini, strlen($catalog));

    $miniFat = array_fill(0, $sector / 4, $FREE);
    $miniFat[0] = 1;             // summary chain: mini sector 0 -> 1
    $miniFat[1] = $ENDOFCHAIN;
    $miniFat[2] = $ENDOFCHAIN;   // catalog: single mini sector

    $root = $dirEntry('Root Entry', 5, 3, $sector, true);
    $cat = $dirEntry('Catalog', 2, 2, strlen($catalog));
    $summ = $dirEntry("\x05SummaryInformation", 2, 0, strlen($summary));
    if ($unterminatedName) {
        // 32 nonzero UTF-16 units (0x0041): no NUL anywhere in d_name.
        $fourth = str_repeat("\0", 128);
        $rawName = str_repeat("\x41\x00", 32);
        $fourth = substr_replace($fourth, $rawName, 0, 64);
        $fourth = substr_replace($fourth, $u16(64), 64, 2);
        $fourth[66] = chr(2);
        $fourth[67] = chr(1);
        $fourth = substr_replace($fourth, pack('i', -1), 68, 4);
        $fourth = substr_replace($fourth, pack('i', -1), 72, 4);
        $fourth = substr_replace($fourth, pack('i', -1), 76, 4);
    } else {
        $fourth = str_repeat("\0", 128);
    }
    $directory = $root . $cat . $summ . $fourth;

    $fat = array_fill(0, $sector / 4, $FREE);
    $fat[0] = $FATSECT;
    $fat[1] = $ENDOFCHAIN;
    $fat[2] = $ENDOFCHAIN;
    $fat[3] = $ENDOFCHAIN;

    $h = str_repeat("\0", 512);
    $h = substr_replace($h, hex2bin('D0CF11E0A1B11AE1'), 0, 8);
    $h = substr_replace($h, $u16(0x003E), 24, 2);
    $h = substr_replace($h, $u16(0x0003), 26, 2);
    $h = substr_replace($h, $u16(0xFFFE), 28, 2);
    $h = substr_replace($h, $u16(9), 30, 2);
    $h = substr_replace($h, $u16(6), 32, 2);
    $h = substr_replace($h, $u32(1), 44, 4);          // FAT sectors
    $h = substr_replace($h, $u32(2), 48, 4);          // first directory
    $h = substr_replace($h, $u32(4096), 56, 4);      // mini cutoff
    $h = substr_replace($h, $u32(1), 60, 4);          // first miniFAT
    $h = substr_replace($h, $u32(1), 64, 4);          // miniFAT sectors
    $h = substr_replace($h, $u32($ENDOFCHAIN), 68, 4); // first DIFAT
    $h = substr_replace($h, $u32(0), 72, 4);          // DIFAT count
    $h = substr_replace($h, $u32(0), 76, 4);          // DIFAT[0]
    for ($k = 1; $k < 109; $k++) {
        $h = substr_replace($h, $u32($FREE), 76 + 4 * $k, 4);
    }

    $out = $h;
    foreach ($fat as $v) {
        $out .= $u32($v);
    }
    foreach ($miniFat as $v) {
        $out .= $u32($v);
    }
    $out .= $directory;
    $out .= $miniStream;
    return $out;
}

$finfo = finfo_open(FILEINFO_MIME_TYPE);
foreach ([true, false] as $unterminated) {
    $mime = finfo_buffer($finfo, build_cdf($unterminated));
    var_dump(is_string($mime) && str_contains($mime, 'ms-office'));
}
?>
--EXPECT--
bool(true)
bool(true)
