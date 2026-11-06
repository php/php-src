#!/usr/bin/env php
<?php

declare(strict_types=1);

namespace Phpsrc\Ci\DownloadBundled;

// How to add a new bundled source?
// 1. add a new entry for the source into $bundles variable below
// 2. run this make-workflow-file.php generator
// 3. edit the generated .github/scripts/download-bundled/<name>.sh file, see and be consistent with other files

// How to run this make-workflow-file.php generator?
// ```
// docker run --rm -v "$(git rev-parse --show-toplevel)":/h php:latest /h/.github/scripts/download-bundled/make-workflow-file.php
// ```

// How to update/download specific bundled source?
// 1. edit the bundled version inside relevant .github/scripts/download-bundled/<name>.sh file
// 2. run:
// ```
// docker run --rm -v "$(git rev-parse --show-toplevel)":/f alpine:latest sh -c "apk add git && /f/.github/scripts/download-bundled/<name>.sh"
// ```
// 3. review git changes and commit the changes

$bundles = [
    new Bundle('boost.context', ['Zend/asm']),
    new Bundle('XSSE', [], ['Zend/zend_simd.h']),
    new Bundle('JIT IR', ['ext/opcache/jit/ir']),
    new Bundle('xxHash', ['ext/hash/xxhash']),
    new Bundle('PCRE2', ['ext/pcre/pcre2lib']),
    new Bundle('uriparser', ['ext/uri/uriparser']),
];

class Bundle
{
    /**
     * @param list<string> $directories
     * @param list<string> $files
     */
    public function __construct(
        public string $name,
        public array $directories,
        public array $files = []
    ) {}

    public function getNameForPath(): string
    {
        return preg_replace('~\W+~', '-', strtolower($this->name));
    }
}

class Generator
{
    /**
     * @param list<Bundle> $bundles
     */
    public function __construct(
        public array $bundles
    ) {}

    protected function getRepoDirectory(): string
    {
        return dirname(__DIR__, 3);
    }

    protected function indentString(string $value, int $levels, bool $inclFirstLine): string
    {
        return preg_replace(
            '~' . ($inclFirstLine ? '^|' : '') . '(?<=\n)~',
            str_repeat('  ', $levels),
            $value
        );
    }

    /**
     * @param mixed $data
     */
    protected function encodeYml($data): string
    {
        if (is_array($data)) {
            $isList = array_is_list($data);
            $resParts = [];
            foreach ($data as $k => $v) {
                $kEncoded = $isList
                    ? '-'
                    : $this->encodeYml($k) . ':';
                $vEncoded = $this->encodeYml($v);

                $resParts[] = $kEncoded
                    . (!$isList && is_array($v) && $v !== [] ? "\n  " : ' ')
                    . (is_array($v) ? $this->indentString($vEncoded, 1, false) : $vEncoded);
            }

            return implode("\n", $resParts);
        }

        if (preg_match('~^(\w+|\$\{\{[^\}]+\}\})$~', $data)) {
            return $data;
        }

        return strpos($data, "\n") !== false
            ? '|' . "\n" . $this->indentString($data, 1, true)
            : '\'' . str_replace('\'', '\'\'', $data) . '\'';
    }

    public function makeWorkflowFile(): void
    {
        $content = <<<'YAML'
            name: Verify Bundled Files

            on:
              push:
                paths: &paths
                  %paths%
              pull_request:
                paths: *paths
              schedule:
                - cron: "0 1 * * *"
              workflow_dispatch: ~

            permissions:
              contents: read

            jobs:
              VERIFY_BUNDLED_FILES:
                if: github.repository == 'php/php-src' || github.event_name == 'workflow_dispatch'
                name: Verify Bundled Files
                runs-on: ubuntu-24.04
                steps:
                  - name: git checkout
                    uses: actions/checkout@v7

                  - name: Detect changed files
                    if: ${{ github.event_name == 'push' || github.event_name == 'pull_request' }}
                    uses: dorny/paths-filter@v3
                    id: changes
                    with:
                      filters: %filters%

                  %steps%

            YAML;

        $paths = [
            '.github/scripts/download-bundled/**',
        ];
        foreach ($this->bundles as $bundle) {
            foreach ($this->makeFiltersForDornyPathsFilterAction($bundle) as $p) {
                if (str_starts_with($p, '.github/scripts/download-bundled/')) {
                    continue;
                }

                $paths[] = $p;
            }
        }
        $content = str_replace('%paths%', $this->indentString($this->encodeYml($paths), 3, false), $content);

        $filters = [];
        foreach ($this->bundles as $bundle) {
            $filters[$bundle->getNameForPath()] = $this->makeFiltersForDornyPathsFilterAction($bundle);
        }
        $content = str_replace('%filters%', $this->indentString($this->encodeYml($this->encodeYml($filters)), 5, false), $content);

        $steps = [];
        foreach ($this->bundles as $bundle) {
            $steps[] = [
                'name' => $bundle->name,
                'if' => '${{ !cancelled() && (steps.changes.outputs.' . $bundle->getNameForPath() . ' == \'true\' || github.event_name == \'schedule\' || github.event_name == \'workflow_dispatch\') }}',
                'run' => implode("\n", [
                    'echo "::group::Download"',
                    '.github/scripts/download-bundled/' . $bundle->getNameForPath() . '.sh',
                    'echo "::endgroup::"',
                    'echo "::group::Verify files"',
                    ...array_map(static fn ($v) => '.github/scripts/test-directory-unchanged.sh \'' . $v . '\'', array_merge($bundle->directories, $bundle->files)),
                    'echo "::endgroup::"',
                ]),
            ];
        }
        $content = str_replace('%steps%', $this->indentString($this->encodeYml($steps), 3, false), $content);

        file_put_contents($this->getRepoDirectory() . '/.github/workflows/verify-bundled-files.yml', $content);
    }

    protected function makeFiltersForDornyPathsFilterAction(Bundle $bundle): array
    {
        return [
            '.github/scripts/download-bundled/' . $bundle->getNameForPath() . '.*',
            ...array_map(static fn ($v) => $v . '/**', $bundle->directories),
            ...$bundle->files,
        ];
    }

    public function makeDownloadScriptHeaders(): void
    {
        foreach ($this->bundles as $bundle) {
            $this->makeDownloadScriptHeader($bundle);
        }
    }

    protected function makeDownloadScriptHeader(Bundle $bundle): void
    {
        $scriptPath = $this->getRepoDirectory() . '/.github/scripts/download-bundled/' . $bundle->getNameForPath() . '.sh';

        $content = !file_exists($scriptPath)
            ? "# TODO\n"
            : file_get_contents($scriptPath);

        $header = <<<'YAML'
            #!/bin/sh
            set -ex
            cd "$(dirname "$0")/../../.."

            tmp_dir=%tmp_dir%
            rm -rf "$tmp_dir"


            YAML;

        $header = str_replace('%tmp_dir%', '/tmp/php-src-download-bundled/' . $bundle->getNameForPath(), $header);

        if (!str_starts_with($content, $header)) {
            $content = $header . $content;
        }

        file_put_contents($scriptPath, $content);
    }
}

$generator = new Generator($bundles);
$generator->makeWorkflowFile();
$generator->makeDownloadScriptHeaders();
