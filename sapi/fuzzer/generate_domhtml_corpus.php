<?php
/* Assembles seeds for sapi/fuzzer/fuzzer-dom-html.c.
 *
 * The fuzzer feeds raw HTML bytes into Dom\HTMLDocument::createFromString() and
 * serializes the result via saveHtml(). Seeds are the .html fixtures shipped with
 * ext/dom tests, plus a few hand-built edge-case documents.
 */
function generate_domhtml_corpus(string $corpusDir, array $testDirs, int $maxLen = 512 * 1024) {
    @mkdir($corpusDir, 0777, true);

    $n = 0;
    foreach ($testDirs as $testDir) {
        if (!is_dir($testDir)) { continue; }
        $it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($testDir));
        foreach ($it as $file) {
            if (!preg_match('/\.html$/i', (string) $file)) { continue; }
            $content = file_get_contents((string) $file);
            if ($content === false || $content === '' || strlen($content) > $maxLen) { continue; }
            file_put_contents(sprintf('%s/domhtml_%03d.html', $corpusDir, $n++), $content);
        }
    }

    /* Hand-built edge cases not present as fixtures. */
    $extra = [
        '<!DOCTYPE html><html><head><meta charset="windows-1252"><title>t</title></head>'
            . '<body><p>caf&#233; &amp; &quot;quotes&quot; &#0;</p></body></html>',
        '<body><svg><foreignObject><div xmlns="http://www.w3.org/1999/xhtml">x</div></foreignObject></svg>'
            . '<math><mi>y</mi></math><template><tr><td>t</td></tr></template></body>',
        '<table><tbody><tr><td><table><tr><td>deep</td></tr></table></td></tr></tbody></table>'
            . '<p>unclosed <b>bold <i>both <span>spans',
        '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN"><html><!-- comment -- with -- dashes -->'
            . '<head><script>if (1 < 2) { x = "</div>"; }</script><style>p:before { content: "</"; }</style></head></html>',
        "\xEF\xBB\xBF<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
            . '</head><body>' . str_repeat('<div class="x" data-y="1">d</div>', 8) . '</body></html>',
    ];
    foreach ($extra as $content) {
        file_put_contents(sprintf('%s/domhtml_extra_%02d.html', $corpusDir, $n++), $content);
    }
}
