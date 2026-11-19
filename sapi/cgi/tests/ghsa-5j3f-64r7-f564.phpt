--TEST--
GHSA-5j3f-64r7-f564 (PATH_INFO underflow when the stripped suffix is longer than PATH_INFO)
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php
include "include.inc";

$php = get_cgi_path();
$script = __DIR__ . "/ghsa-5j3f-64r7-f564.target.php";
file_put_contents($script, '<?php var_dump($_SERVER["PATH_INFO"] ?? null, $_SERVER["ORIG_PATH_INFO"] ?? null);');

function run_cgi(string $php, array $env): void {
    /* Keep what is needed to start the binary (e.g. sanitizer runtime on Windows). */
    $base_env = array_filter([
        "PATH" => getenv("PATH"),
        "SystemRoot" => getenv("SystemRoot"),
    ], 'is_string');
    $p = proc_open(
        [$php, "-n", "-d", "cgi.fix_pathinfo=1"],
        [0 => ["pipe", "r"], 1 => ["pipe", "w"]],
        $pipes,
        null,
        $env + ["REDIRECT_STATUS" => "1", "REQUEST_METHOD" => "GET"] + $base_env
    );
    fclose($pipes[0]);
    $out = stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    proc_close($p);
    echo substr($out, strpos($out, "\r\n\r\n") + 4);
}

echo "Suffix shorter than PATH_INFO\n";
run_cgi($php, ["SCRIPT_FILENAME" => "$script/b", "PATH_INFO" => "/a/b"]);

echo "Suffix longer than PATH_INFO\n";
run_cgi($php, ["SCRIPT_FILENAME" => "$script/xxxxxxxxxxxxxxxx", "PATH_INFO" => "/x"]);

echo "Suffix taken from PATH_TRANSLATED\n";
run_cgi($php, [
    "SCRIPT_FILENAME" => $php,
    "PATH_TRANSLATED" => "$script/xxxxxxxxxxxxxxxx",
    "REDIRECT_URL" => "/redirected",
    "PATH_INFO" => "/x",
]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/ghsa-5j3f-64r7-f564.target.php");
?>
--EXPECT--
Suffix shorter than PATH_INFO
string(2) "/b"
string(4) "/a/b"
Suffix longer than PATH_INFO
string(2) "/x"
NULL
Suffix taken from PATH_TRANSLATED
string(2) "/x"
NULL
