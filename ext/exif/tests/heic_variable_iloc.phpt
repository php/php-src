--TEST--
HEIC EXIF metadata with variable-width iloc entries
--EXTENSIONS--
exif
--FILE--
<?php
foreach (["heic_variable_iloc.heic", "heic_variable_iloc_extent_count.heic"] as $file) {
    $data = @exif_read_data(__DIR__ . "/" . $file, null, true, true);
    echo $file, ": ", $data["IFD0"]["Make"] ?? "unreadable", "\n";
}
?>
--EXPECT--
heic_variable_iloc.heic: ZopiTech
heic_variable_iloc_extent_count.heic: ZopiTech Lab
