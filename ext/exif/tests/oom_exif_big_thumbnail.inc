<?php

/* Writes a TIFF whose thumbnail IFD points at $size bytes that really are in the
 * file. */
function oom_exif_write_big_thumbnail(string $path, int $size): void
{
    $ifd1_off    = 8 + 2 + 2*12 + 4;
    $payload_off = $ifd1_off + 2 + 2*12 + 4;

    $t  = "II*\x00" . pack('V', 8);
    $t .= pack('v', 2);
    $t .= pack('vvVV', 0x0100, 4, 1, 1);                // ImageWidth
    $t .= pack('vvVV', 0x0101, 4, 1, 1);                // ImageLength
    $t .= pack('V', $ifd1_off);                         // next IFD = thumbnail IFD
    $t .= pack('v', 2);
    $t .= pack('vvVV', 0x0111, 4, 1, $payload_off);     // StripOffsets -> real data
    $t .= pack('vvVV', 0x0117, 4, 1, $size);            // StripByteCounts
    $t .= pack('V', 0);

    $fp = fopen($path, 'wb');
    fwrite($fp, $t);
    $chunk = str_repeat("\xAB", 1024 * 1024);
    for ($written = 0; $written < $size; $written += strlen($chunk)) {
        fwrite($fp, $chunk);
    }
    fclose($fp);
}

/* Writes a TIFF with a single tag of $count elements of $format, whose value
 * really is present in the file. $bytes_per is the format's element size. */
function oom_exif_write_big_tag(string $path, int $tag, int $format, int $count, int $bytes_per = 1): void
{
    $payload_off = 8 + 2 + 12 + 4;
    $t  = "II*\x00" . pack('V', 8);
    $t .= pack('v', 1);
    $t .= pack('vvVV', $tag, $format, $count, $payload_off);
    $t .= pack('V', 0);

    $fp = fopen($path, 'wb');
    fwrite($fp, $t);
    $size = $count * $bytes_per;
    $chunk = str_repeat("\x01", 1024 * 1024);
    for ($written = 0; $written < $size; $written += strlen($chunk)) {
        fwrite($fp, substr($chunk, 0, min(strlen($chunk), $size - $written)));
    }
    fclose($fp);
}
