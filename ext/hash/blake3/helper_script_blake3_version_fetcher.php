<?php
declare(strict_types = 1);

// this script is intended to help developers update to newer versions of
// the upstream "official blake3 optimized c implementation"..
// not something to be used often
const BLAKE3_TARGET_VERSION = "0.3.7";

/**
 * fetch url with user-agent
 * (file_get_contents doesn't use user-agent, and api.github.com block people without UA)
 *
 * @param string $url
 * @throws \LogicException
 * @return string
 */
function fetch(string $url): string
{
    $ch = curl_init();
    curl_setopt_array($ch, array(
        CURLOPT_URL => $url,
        CURLOPT_USERAGENT => 'php-src/ext/hash/blake3/helper_script bot',
        CURLOPT_ENCODING => '',
        CURLOPT_RETURNTRANSFER => 1
    ));
    $ret = curl_exec($ch);
    if (curl_errno($ch) !== CURLE_OK || curl_getinfo($ch, CURLINFO_HTTP_CODE) !== 200) {
        throw new \LogicException();
    }
    curl_close($ch);
    return $ret;
}

$files = json_decode(fetch("https://api.github.com/repos/BLAKE3-team/BLAKE3/contents/c?ref=" . urlencode(BLAKE3_TARGET_VERSION)), true);
foreach ($files as $file) {
    // $file looks like:
    array(
        'name' => 'blake3_avx512.c',
        'path' => 'c/blake3_avx512.c',
        'sha' => '77a5c385cb897f712d55dd884d8ae54f14ed2570',
        'size' => 47960,
        'url' => 'https://api.github.com/repos/BLAKE3-team/BLAKE3/contents/c/blake3_avx512.c?ref=0.3.7',
        'html_url' => 'https://github.com/BLAKE3-team/BLAKE3/blob/0.3.7/c/blake3_avx512.c',
        'git_url' => 'https://api.github.com/repos/BLAKE3-team/BLAKE3/git/blobs/77a5c385cb897f712d55dd884d8ae54f14ed2570',
        'download_url' => 'https://raw.githubusercontent.com/BLAKE3-team/BLAKE3/0.3.7/c/blake3_avx512.c',
        'type' => 'file',
        '_links' => array(
            'self' => 'https://api.github.com/repos/BLAKE3-team/BLAKE3/contents/c/blake3_avx512.c?ref=0.3.7',
            'git' => 'https://api.github.com/repos/BLAKE3-team/BLAKE3/git/blobs/77a5c385cb897f712d55dd884d8ae54f14ed2570',
            'html' => 'https://github.com/BLAKE3-team/BLAKE3/blob/0.3.7/c/blake3_avx512.c'
        )
    );
    $name = $file["name"];
    if ($file["type"] !== "file" || ! preg_match('/^blake3.*\\.(?:c|h|S|asm)$/', $name)) {
        // the dir contains several things we're not interested in.
        continue;
    }
    var_dump($file["name"]);
    $content = fetch($file['download_url']);
    file_put_contents($name, $content, LOCK_EX);
}
