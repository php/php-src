<?php

function create_data_from_utf8($id, $item = "", $cp = 65001)
{
    $prefix = dirname(__FILE__) . DIRECTORY_SEPARATOR . $id;

    if (!is_dir($prefix)) {
        mkdir($prefix);
    }

    /* Using a UTF-8 encoded filenames if !zend.multibyte, otherwise files in retrospective charset. */
    switch ($id) {

        case "file_mb3":
            $content = "<?php

echo \"hello there from the include\n\";";
            create_verify_file($prefix, "českýtestování.inc", $content);
            break;

        case "bug71509":
            $content = "that's my file with swedish filename
";
            create_verify_file($prefix, $item, $content);
            break;

        case "file_mb4":
            create_verify_file($prefix, "š.txt", "");
            break;

        case "file_cp1252":
            $content = "hallo
";
            create_verify_file($prefix, "tschüß", $content);
            break;

        case "file2_cp1252":
            create_verify_file($prefix, $item, "hola");
            break;

        case "file2_mb":
            create_verify_file($prefix, "Ελλάδα.txt", "");
            break;

        case "file_cp1251":
            create_verify_file($prefix, $item, "opened an utf8 filename for reading", $cp);
            break;

        case "file_cp1255":
        case "file_cp1254":
        case "file_cp1253":
        case "file_kartuli":
        case "file_cp1250":
        case "file_cp1256":
        case "file_cp874":
        case "file_big5":
        case "file_cp936":
        case "file_cp932":
        case "file_eucjp":
        case "file_mb":
            $content = "reading file wihh multibyte filename
";
            create_verify_file($prefix, $item, $content, $cp);
            break;

        case "dir_mb":
            create_verify_dir($prefix, "テストマルチバイト・パス42");
            break;

        case "dir_cp1255":
        case "dir_cp1254":
        case "dir_cp1253":
        case "dir_kartuli":
        case "dir_cp1250":
        case "dir_cp1256":
        case "dir_cp874":
        case "dir_cp932":
        case "dir_cp936":
        case "dir_eucjp":
        case "dir2_cp1252":
        case "dir_cp1252":
        case "dir_cp1251":
        case "dir_big5":
        case "dir":
            create_verify_dir($prefix, $item, $cp);
            break;

        case "file":
            create_verify_file($prefix, $item, "", $cp);
            break;
    }

    return $prefix;
}
