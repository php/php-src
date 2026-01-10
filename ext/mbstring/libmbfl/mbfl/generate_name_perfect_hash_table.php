<?php

function read_encoding_pointer_array(): array {
    // read the encoding pointer array
    $file_content = file_get_contents(__DIR__ . '/mbfl_encoding.c');
    $pattern = '/static const mbfl_encoding \*mbfl_encoding_ptr_list\[\][\s\S]*?\{([^}]*)\};/';
    preg_match($pattern, $file_content, $matches);
    assert(isset($matches[1]));
    $array = explode(",\n", $matches[1]);
    $array = array_map(function ($item) {
        return trim($item, "&\n\t ");
    }, $array);
    array_pop($array); // Remove NULL
    return $array;
}

function search_struct_in_file(array &$result, $struct_names, $file_path)
{
    $fileContent = file_get_contents($file_path);
    $pattern = '/const mbfl_encoding\s+(' . implode('|', $struct_names) . ')\s* = {([^}]*)}/';
    preg_match_all($pattern, $fileContent, $matches, PREG_SET_ORDER);
    foreach ($matches as $match) {
        $current_struct_name = $match[1];
        $struct_definition = $match[2];
        // Note: name is the second file
        $name = explode(',', $struct_definition)[1];
        $result[$current_struct_name] = trim($name, " \n\t\"");
    }
}

function search_struct_in_dir($struct_names): array
{
    $result = [];
    foreach (glob(__DIR__ . "/../**/*.c") as $file) {
        search_struct_in_file($result, $struct_names, $file);
    }
    return $result;
}

$encoding_pointer_array = read_encoding_pointer_array();
$encoding_pointer_array_name_mapping = search_struct_in_dir($encoding_pointer_array);

// The single byte encodings are generated and cannot be found in dedicated generated structs
$fixed_encodings = [
    'mbfl_encoding_cp1251' => 'Windows-1251',
    'mbfl_encoding_cp1252' => 'Windows-1252',
    'mbfl_encoding_cp1254' => 'Windows-1254',
    'mbfl_encoding_8859_1' => 'ISO-8859-1',
    'mbfl_encoding_8859_2' => 'ISO-8859-2',
    'mbfl_encoding_8859_3' => 'ISO-8859-3',
    'mbfl_encoding_8859_4' => 'ISO-8859-4',
    'mbfl_encoding_8859_5' => 'ISO-8859-5',
    'mbfl_encoding_8859_6' => 'ISO-8859-6',
    'mbfl_encoding_8859_7' => 'ISO-8859-7',
    'mbfl_encoding_8859_8' => 'ISO-8859-8',
    'mbfl_encoding_8859_9' => 'ISO-8859-9',
    'mbfl_encoding_8859_10' => 'ISO-8859-10',
    'mbfl_encoding_8859_13' => 'ISO-8859-13',
    'mbfl_encoding_8859_14' => 'ISO-8859-14',
    'mbfl_encoding_8859_15' => 'ISO-8859-15',
    'mbfl_encoding_8859_16' => 'ISO-8859-16',
    'mbfl_encoding_cp866' => 'CP866',
    'mbfl_encoding_cp850' => 'CP850',
    'mbfl_encoding_koi8r' => 'KOI8-R',
    'mbfl_encoding_koi8u' => 'KOI8-U',
    'mbfl_encoding_armscii8' => 'ArmSCII-8',
    'mbfl_encoding_ascii' => 'ASCII',
];

// Add the fixed encodings
foreach ($fixed_encodings as $encoding_pointer => $encoding_name) {
    $encoding_pointer_array_name_mapping[$encoding_pointer] = $encoding_name;
}

// Consistency check: all of the encoding pointer array entries should be found
foreach ($encoding_pointer_array as $encoding_pointer) {
    assert(isset($encoding_pointer_array_name_mapping[$encoding_pointer]), "Missing entry for $encoding_pointer");
}

$ordered_name_list = array_map(function ($encoding_pointer) use ($encoding_pointer_array_name_mapping) {
    return $encoding_pointer_array_name_mapping[$encoding_pointer];
}, $encoding_pointer_array);

// Write out ordered name list, and invoke gperf for computing the perfect hash table
file_put_contents(__DIR__ . '/encodings.txt', implode("\n", $ordered_name_list));
ob_start();
passthru('gperf ' . escapeshellarg(__DIR__ . '/encodings.txt') . ' --readonly-tables --null-strings --ignore-case -m 1000');
$output = ob_get_clean();
@unlink(__DIR__ . '/encodings.txt');

// Find asso_values array in $output
$pattern = '/static const unsigned char asso_values\[\] =([^}]*)\};/';
preg_match($pattern, $output, $matches);
assert(isset($matches[1]));
$asso_values = trim($matches[1], "\t \n{");
echo "===--- Copy and paste the following values in the asso_values array in mbfl_encoding.c ---===\n";
echo $asso_values, "\n";

// Find word_list array in $output
$pattern = '/static const char \* const wordlist\[\] =([^}]*)\};/';
preg_match($pattern, $output, $matches);
assert(isset($matches[1]));
$word_list = trim($matches[1], "\t \n{");
$word_list = str_replace('(char*)0', '-1', $word_list);
foreach ($encoding_pointer_array_name_mapping as $key => $value)
{
    $index = array_search($key, $encoding_pointer_array);
    $word_list = str_replace("\"$value\"", $index, $word_list);
}

echo "===--- Copy and paste the following values in the mbfl_encoding_ptr_list_after_hashing array in mbfl_encoding.c ---===\n";
echo $word_list, "\n";
